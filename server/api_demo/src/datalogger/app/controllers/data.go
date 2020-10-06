package controllers

import (
	"github.com/revel/revel"
	"datalogger/app"
	"datalogger/app/models"
)

type Data struct {
	*revel.Controller
}

func (c Data) Stations() revel.Result{
	stationList := getAllStation()
	return c.RenderJSON(stationList)
}

func (c Data) LastestData(code string) revel.Result{
	data := getLastestData(code)
	if data != nil{
		return c.RenderJSON(data)
	}
	c.Response.Status = 404
	return c.RenderJSON(map[string]string{"detail": "Data not found."})
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

	sql := "SELECT time, payload FROM data WHERE station_code=$1 ORDER BY time DESC LIMIT 1"

	row := app.DB.QueryRow(sql, station)
	err := row.Scan(&data.Timestamp, &data.Payload)
	if err != nil{
		return nil
	}

	return &data 
}