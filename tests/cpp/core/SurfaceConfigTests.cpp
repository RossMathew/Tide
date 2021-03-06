/*********************************************************************/
/* Copyright (c) 2018, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
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

#define BOOST_TEST_MODULE SurfaceConfigTests

#include "configuration/SurfaceConfigValidator.h"

#include "StandardScreenConfigs.h"

#include <boost/test/unit_test.hpp>

namespace
{
const auto screenWidth = 1.6;  // m
const auto screenHeight = 0.9; // m
const auto tolerance = 0.005;  // 5 mm
const auto excess = 0.01;      // 1 cm
const auto pixelExcess = 7;
}

SurfaceConfig fullHDSurface()
{
    auto surface = SurfaceConfig();
    surface.displayWidth = 1920;
    surface.displayHeight = 1080;
    return surface;
}

BOOST_AUTO_TEST_CASE(test_surface_without_dimensions)
{
    const auto surface = fullHDSurface();
    const auto validator = SurfaceConfigValidator{surface};
    BOOST_CHECK_NO_THROW(validator.validateDimensions());
}

BOOST_AUTO_TEST_CASE(test_surface_with_valid_dimensions)
{
    auto surface = fullHDSurface();
    const auto validator = SurfaceConfigValidator{surface};

    surface.dimensions = QSizeF{screenWidth, screenHeight};
    BOOST_CHECK_NO_THROW(validator.validateDimensions());

    surface.dimensions = QSizeF{screenWidth, screenHeight + tolerance};
    BOOST_CHECK_NO_THROW(validator.validateDimensions());

    surface.dimensions = QSizeF{screenWidth + tolerance, screenHeight};
    BOOST_CHECK_NO_THROW(validator.validateDimensions());

    surface = standardWallWithDimension();
    BOOST_CHECK_NO_THROW(validator.validateDimensions());

    surface = narrowWallWithDimension();
    BOOST_CHECK_NO_THROW(validator.validateDimensions());

    surface = wideThinWallWithDimension();
    BOOST_CHECK_NO_THROW(validator.validateDimensions());

    surface = largeProjectionSurfaceWithDimensions();
    BOOST_CHECK_NO_THROW(validator.validateDimensions());
}

BOOST_AUTO_TEST_CASE(test_surface_with_invalid_dimensions)
{
    auto surface = fullHDSurface();
    const auto validator = SurfaceConfigValidator{surface};

    surface.dimensions = QSizeF{4.0, 3.0};
    BOOST_CHECK_THROW(validator.validateDimensions(), dimensions_mismatch);

    surface.dimensions = QSizeF{screenWidth + excess, screenHeight};
    BOOST_CHECK_THROW(validator.validateDimensions(), dimensions_mismatch);

    surface.dimensions = QSizeF{screenWidth, screenHeight + excess};
    BOOST_CHECK_THROW(validator.validateDimensions(), dimensions_mismatch);
}

BOOST_AUTO_TEST_CASE(test_surface_with_incorrect_pixel_count)
{
    auto surface = fullHDSurface();
    surface.dimensions = QSizeF{screenWidth, screenHeight};
    const auto validator = SurfaceConfigValidator{surface};

    surface.displayHeight = 1080 + pixelExcess;
    BOOST_CHECK_THROW(validator.validateDimensions(), dimensions_mismatch);

    surface.displayWidth = 1080;

    surface.displayHeight = 1920 + pixelExcess;
    BOOST_CHECK_THROW(validator.validateDimensions(), dimensions_mismatch);
}

BOOST_AUTO_TEST_CASE(test_error_message)
{
    auto surface = fullHDSurface();
    surface.dimensions = QSizeF{screenWidth, screenHeight + excess};
    const auto validator = SurfaceConfigValidator{surface};

    const auto message =
        "The aspect ratio of the surface in pixels (1920 x 1080) -> 1.77778 "
        "does not match that of the dimensions in meters (1.6 x 0.91) -> "
        "1.75824";
    try
    {
        validator.validateDimensions();
        BOOST_CHECK(!"Expected dimensions_mismatch exception");
    }
    catch (const dimensions_mismatch& e)
    {
        BOOST_CHECK_EQUAL(e.what(), message);
    }
}
