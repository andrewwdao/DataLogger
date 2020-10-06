-- *************** SqlDBM: PostgreSQL ****************;
-- ***************************************************;

CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;

-- ************************************** "stations"

CREATE TABLE IF NOT EXISTS "stations"
(
 "station_code"       varchar(10) NOT NULL,
 "station_address"    varchar(100) NOT NULL,
 "station_sim"        varchar(12) NOT NULL,
 "station_longitude"  decimal(9,6) NOT NULL,
 "station_latitude"   decimal(8,6) NOT NULL,
 "station_connection" boolean NOT NULL,
 "station_display"    boolean NOT NULL,
 "station_ttl"        integer NOT NULL,
 "station_tts"        integer NOT NULL,
 "station_dp"         integer NOT NULL,
 "sensor_thresholds"  json NOT NULL,
 "sensor_parameters"  json NOT NULL,
 "device_parameters"  json NOT NULL,
 CONSTRAINT "PK_location" PRIMARY KEY ( "station_code" ),
 CONSTRAINT "ind_74" UNIQUE ( "station_code" )
);

-- ************************************** "data"

CREATE TABLE IF NOT EXISTS "data"
(
 "time"         timestamp with time zone NOT NULL,
 "station_code" varchar(10) NOT NULL,
 "payload"      json NOT NULL,
 CONSTRAINT "PK_timeseries data" PRIMARY KEY ( "time", "station_code" ),
 CONSTRAINT "FK_24" FOREIGN KEY ( "station_code" ) REFERENCES "stations" ( "station_code" ) ON DELETE CASCADE
);

CREATE INDEX "fkIdx_24" ON "data"
(
 "station_code"
);


SELECT create_hypertable('data', 'time');