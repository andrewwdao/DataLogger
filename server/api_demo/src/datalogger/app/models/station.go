package models 

type Station struct{
	StationCode string `json:"station_code"`
	StationAddress string `json:"station_address"`
	StationLongitude float64 `json:"station_longitude"`
	StationLatitude float64 `json:"station_latitude"`
	StationParams []string `json:"station_params"`
}