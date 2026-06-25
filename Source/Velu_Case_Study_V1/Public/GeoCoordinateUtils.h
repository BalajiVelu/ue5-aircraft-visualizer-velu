#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"


namespace GeoCoordinateUtils
{
    // WGS84 Earth radius (meters)
    static constexpr double EARTH_RADIUS = 6371000.0;
    static constexpr double DEG_TO_RAD = PI / 180.0;
    static constexpr double RAD_TO_DEG = 180.0 / PI;

    /**
     * Convert geodetic coordinates (latitude, longitude, altitude) to ENU coordinates
     * relative to a reference origin point.
     * 
     * @param Latitude 
     * @param Longitude 
     * @param Altitude 
     * @param RefLatitude 
     * @param RefLongitude 
     * @param RefAltitude 
     * @return 
     */
    inline FVector GeoToENU(
        double Latitude, double Longitude, double Altitude,
        double RefLatitude, double RefLongitude, double RefAltitude)
    {
        // Convert all to radians
        double LatRad = Latitude * DEG_TO_RAD;
        double LonRad = Longitude * DEG_TO_RAD;
        double RefLatRad = RefLatitude * DEG_TO_RAD;
        double RefLonRad = RefLongitude * DEG_TO_RAD;

        // Calculate differences in radians
        double DeltaLat = LatRad - RefLatRad;
        double DeltaLon = LonRad - RefLonRad;
        double DeltaAlt = Altitude - RefAltitude;

        // Earth radius at the reference latitude (accounts for Earth's oblate shape)
        double CosRefLat = FMath::Cos(RefLatRad);
        double SinRefLat = FMath::Sin(RefLatRad);

        // Radius of curvature in prime vertical (N) and meridian (M)
        double N = EARTH_RADIUS / FMath::Sqrt(1.0 - 0.0067 * SinRefLat * SinRefLat); // WGS84 flattening
        double M = EARTH_RADIUS * (1.0 - 0.0067) / FMath::Sqrt(1.0 - 0.0067 * SinRefLat * SinRefLat);

        // ENU coordinates in meters
        double East = (N + RefAltitude) * CosRefLat * DeltaLon;
        double North = (M + RefAltitude) * DeltaLat;
        double Up = DeltaAlt;

        // Convert to UE4 coordinates (scale to cm)
        return FVector(
            East * 100.0,   // X = East
            North * 100.0,  // Y = North
            Up * 100.0      // Z = Up
        );
    }

    
    inline FVector GeoToENUSimple(
        double Latitude, double Longitude, double Altitude,
        double RefLatitude, double RefLongitude, double RefAltitude)
    {
        double DeltaLat = (Latitude - RefLatitude);
        double DeltaLon = (Longitude - RefLongitude);
        double DeltaAlt = (Altitude - RefAltitude);

        // At reference latitude, calculate meters per degree
        double RefLatRad = RefLatitude * DEG_TO_RAD;
        double MetersPerDegreeLat = 111320.0; // approximately constant
        double MetersPerDegreeLon = 111320.0 * FMath::Cos(RefLatRad);

        double East = DeltaLon * MetersPerDegreeLon;
        double North = DeltaLat * MetersPerDegreeLat;
        double Up = DeltaAlt;

        // Convert to UE4 coordinates (scale to cm)
        return FVector(
            East * 100.0,   // X = East
            North * 100.0,  // Y = North
            Up * 100.0      // Z = Up
        );
    }
}
