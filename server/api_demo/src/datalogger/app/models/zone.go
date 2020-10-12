package models 

type Zone struct{
	ZoneID uint32 `json:"zone_id"`
	ZoneCode string `json:"zone_code"`
	ZoneName string `json:"zone_name"`
}