/**********************************************************************************************************************************************/
#pragma once

/**********************************************************************************************************************************************/
namespace case_study
{

    /**********************************************************************************************************************************************/
#pragma pack(push, 1)
    struct AircraftState
    {
        // Not important for the visualization. Was only for debugging the case-study data sender before providing it to you ;)
        double simulation_time__s;

        // Geocoordinates
        // Start position will not change in this example, but is sent continuously for statelessness
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

    /**********************************************************************************************************************************************/
} // namespace case_study

/**********************************************************************************************************************************************/
