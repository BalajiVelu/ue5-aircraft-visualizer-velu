
#pragma once


namespace case_study
{

    
#pragma pack(push, 1)
    struct AircraftState
    {
        double simulation_time__s;

        // Geocoordinates
        double startposition_latitude__deg;
        double startposition_longitude__deg;
        double startposition_altitude__m_amsl;

        double aircraft_latitude__deg;
        double aircraft_longitude__deg;
        double aircraft_altitude__m_amsl;
    
        // Attitudes range from -pi to pi
        double aircraft_phi__rad;
        double aircraft_theta__rad;
        double aircraft_psi__rad;
        
        // Gimbal attitude is in world coordinates, not local coordinates!
        double gimbal_phi__rad;
        double gimbal_theta__rad;
        double gimbal_psi__rad;
    };
#pragma pack(pop)

    
} 


