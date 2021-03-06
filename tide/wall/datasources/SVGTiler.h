/*********************************************************************/
/* Copyright (c) 2016-2018, EPFL/Blue Brain Project                  */
/*                          Raphael Dumusc <raphael.dumusc@epfl.ch>  */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of Ecole polytechnique federale de Lausanne.          */
/*********************************************************************/

#ifndef SVGTILER_H
#define SVGTILER_H

#include "LodTiler.h"

#include "data/SVG.h"

/**
 * Represent an SVG image as a multi-LOD tiled data source.
 */
class SVGTiler : public LodTiler
{
public:
    /** Constructor. */
    explicit SVGTiler(const QString& uri, const QSize& maxImageSize);

    /** Destructor. */
    ~SVGTiler();

    /**
     * Override for SVG GPU images, threadsafe.
     * @sa CachedDataSource::getTileImage
     */
    ImagePtr getTileImage(uint tileId, deflect::View view) const final;

#if !(TIDE_USE_CAIRO && TIDE_USE_RSVG)
    /** Called exclusively by SVGGpuImage from the GL render thread. */
    ImagePtr renderAndCacheGpuImage(uint tileId, deflect::View view) const;
#endif

private:
    /**
     * Get a tile image which will be cached.
     * Unlike other DataSource classes, this method may need to be called from a
     * thread with an OpenGL context depending on the SVG backend used.
     */
    QImage getCachableTileImage(uint tileId, deflect::View view) const final;
    bool isStereo() const final { return false; }
    const LodTools& _getLodTool() const final { return *_lodTool; }
    SVG& _getSvgForCurrentThread() const;

    std::unique_ptr<SVG> _svg;
    std::unique_ptr<LodTools> _lodTool;

#if (TIDE_USE_CAIRO && TIDE_USE_RSVG)
    mutable QMutex _threadMapMutex;
    mutable std::map<Qt::HANDLE, std::unique_ptr<SVG>> _perThreadSVG;
#endif
};

#endif
