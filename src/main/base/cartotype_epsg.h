/*
cartotype_epsg.h
Copyright (C) 2013-2025 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_types.h>

namespace CartoTypeCore
{

// EPSG codes for projections.

/** The EPSG code for the Web Mercator projection. */
constexpr uint32_t KEpsgWebMercator = 3857;
/** The EPSG code for the Plate Carree projection. */
constexpr uint32_t KEpsgPlateCarree = 4326;

}
