package controllers

import (
	"time"
	"regexp"
	"strconv"
	"github.com/revel/revel"
	"datalogger/app"
	"datalogger/app/models"
	"github.com/lib/pq"
)

type Data struct {
	*revel.Controller
}

func (c Data) Zones() revel.Result{
	zoneList := getAllZone()
    c.Response.Out.Header().Add("Access-Control-Allow-Origin","*")
	return c.RenderJSON(zoneList)
}

func (c Data) Stations() revel.Result{
	stationList := getAllStation()
    c.Response.Out.Header().Add("Access-Control-Allow-Origin","*")
	return c.RenderJSON(stationList)
}

func (c Data) LatestData(code string) revel.Result{
	data := getLastestData(code)
	c.Response.Out.Header().Add("Access-Control-Allow-Origin","*")
	if data != nil{
		return c.RenderJSON(data)
	}
	c.Response.Status = 404
	return c.RenderJSON(map[string]string{"detail": "Data not found"})
}

func (c Data) TimeRangeIntervalData(code string) revel.Result{
	var start, end, interval string

	start = c.Params.Query.Get("start")
	end = c.Params.Query.Get("end")
	interval = c.Params.Query.Get("interval")

	r, _ := regexp.Compile("^[0-9]*$")
	c.Response.Out.Header().Add("Access-Control-Allow-Origin","*")
	if(r.MatchString(start) && r.MatchString(end) && r.MatchString(interval)){
		start_time, _ := strconv.ParseInt(start, 10, 64)
		end_time, _ := strconv.ParseInt(end, 10, 64)
		interval_time, _ := strconv.ParseInt(interval, 10, 64) 
		
		if(interval_time == 0){
			c.Response.Status = 400
			return c.RenderJSON(map[string]string{"detail": "Bad request"})
		}
		data := getTimeRangeIntervalData(code, time.Unix(start_time, 0), time.Unix(end_time, 0), interval_time)
		return c.RenderJSON(data)
	}
	c.Response.Status = 404
	return c.RenderJSON(map[string]string{"detail": "Bad request"})
}

func getAllZone() []models.Zone{
	zoneList := make([]models.Zone, 0)
	
	sql := "SELECT zone_id, zone_code, zone_name FROM zones"

	rows, err := app.DB.Query(sql)
	if err != nil{
		panic(err)
	}
	
	defer rows.Close()

	for rows.Next(){
		var instance models.Zone
		err := rows.Scan(&instance.ZoneID, &instance.ZoneCode, &instance.ZoneName)
		if err != nil{
			panic(err)
		}
		zoneList = append(zoneList, instance)
	}
	return zoneList
}

func getAllStation() []models.Station{
	stationList := make([]models.Station, 0)
	
	sql := "SELECT station_code, station_address, station_longitude, station_latitude, array (select json_object_keys(sensor_parameters))as station_params, zone_id FROM stations;"

	rows, err := app.DB.Query(sql)
	if err != nil{
		panic(err)
	}

	defer rows.Close()

	for rows.Next(){
		var instance models.Station
		err := rows.Scan(&instance.StationCode, &instance.StationAddress, &instance.StationLongitude, 
			&instance.StationLatitude, pq.Array(&instance.StationParams), &instance.ZoneID)
		if err != nil{
			panic(err)
		}
		stationList = append(stationList, instance)
	}
	return stationList
}

func getLastestData(station string) *models.Data{
	var data models.Data
	var timestamp time.Time

	sql := "SELECT time, payload FROM data WHERE station_code=$1 ORDER BY time DESC LIMIT 1"

	row := app.DB.QueryRow(sql, station)
	err := row.Scan(&timestamp, &data.Payload)
	if err != nil{
		return nil
	}
	data.Timestamp = int64(timestamp.Unix())
	return &data 
}

func getTimeRangeDate(station string, start time.Time, end time.Time) []models.Data{
	dataList := make([]models.Data, 0)

	sql := "SELECT time, payload FROM data WHERE station_code=$1 AND time >= $2 AND time < $3"

	rows, err := app.DB.Query(sql, station, start, end)
	if err != nil{
		panic(err)
	}

	defer rows.Close()

	for rows.Next(){
		var timestamp time.Time
		var instance models.Data
		err := rows.Scan(&timestamp, &instance.Payload)
		if err != nil{
			panic(err)
		}
		instance.Timestamp = int64(timestamp.Unix())
		dataList = append(dataList, instance)
	}

	return dataList
}

func getTimeRangeIntervalData(station string, start time.Time, end time.Time, interval int64) []models.Data{
	dataList := make([]models.Data, 0)

	sql := "SELECT time, payload FROM data WHERE station_code=$1 AND time >= $2 AND time < $3 AND MOD((extract(EPOCH from time) - EXTRACT(EPOCH FROM $2))::BIGINT, $4) BETWEEN 0 and 60;"

	rows, err := app.DB.Query(sql, station, start, end, interval)
	if err != nil{
		panic(err)
	}

	defer rows.Close()

	for rows.Next(){
		var timestamp time.Time
		var instance models.Data
		err := rows.Scan(&timestamp, &instance.Payload)
		if err != nil{
			panic(err)
		}
		instance.Timestamp = int64(timestamp.Unix())
		dataList = append(dataList, instance)
	}

	return dataList
}