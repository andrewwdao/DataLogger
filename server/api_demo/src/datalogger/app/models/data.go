package models

import (
	"errors"
	"encoding/json"
	"database/sql/driver"
)

type Data struct{
	Timestamp int64 `json:"timestamp"`
	Payload DataPayload `json:"payload"`
}

type DataPayload map[string]interface{}

func (p DataPayload) Value() (driver.Value, error){
	return json.Marshal(p)
}

func (p *DataPayload) Scan(value interface{}) error{
	b, ok := value.([]byte)
	if !ok{
		return errors.New("type assertion to []byte failed")
	}

	return json.Unmarshal(b, &p)
}
