package controllers

import (
	"time"
	"strconv"
	"github.com/revel/revel"
	"datalogger/app"
	"datalogger/app/models"
)

type Data struct {
	*revel.Controller
}

func (c Data) Stations() revel.Result{
	stationList := getAllStation()
    c.Response.Out.Header().Add("Access-Control-Allow-Origin","*")
	return c.RenderJSON(stationList)
}

func (c Data) LastestData(code string) revel.Result{
	data := getLastestData(code)
	c.Response.Out.Header().Add("Access-Control-Allow-Origin","*")
	if data != nil{
		return c.RenderJSON(data)
	}
	c.Response.Status = 404
	return c.RenderJSON(map[string]string{"detail": "Data not found"})
}

func (c Data) TimeRangeData(code string) revel.Result{
	var start, end int64
	var err error

	if start, err = strconv.ParseInt(c.Params.Query.Get("start"), 10, 32); err != nil {
		c.Response.Status = 400
		return c.RenderJSON(map[string]string{"detail": "Bad request"})
	}
	if end, err = strconv.ParseInt(c.Params.Query.Get("end"), 10, 64); err != nil {
		c.Response.Status = 400
		return c.RenderJSON(map[string]string{"detail": "Bad request"})
	}

	start_time := time.Unix(start, 0)
	end_time := time.Unix(end, 0)

	data := getTimeRangeDate(code, start_time, end_time)

	return c.RenderJSON(data)
}

func getAllStation() []models.Station{
	stationList := make([]models.Station, 0)
	
	sql := "SELECT station_code, station_address, station_longitude, station_latitude FROM stations"

	rows, err := app.DB.Query(sql)
	if err != nil{
		panic(err)
	}

	defer rows.Close()

	for rows.Next(){
		var instance models.Station
		err := rows.Scan(&instance.StationCode, &instance.StationAddress, 
			&instance.StationLongitude, &instance.StationLatitude)
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