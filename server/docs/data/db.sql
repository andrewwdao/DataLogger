-- *************** SqlDBM: PostgreSQL ****************;
-- ***************************************************;

CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;

-- ************************************** "Zones"

CREATE TABLE IF NOT EXISTS "Zones"
(
 "zone_id"   serial NOT NULL,
 "zone_name" varchar(50) NOT NULL,
 "zone_code" varchar(10) NOT NULL,
 CONSTRAINT "PK_zones" PRIMARY KEY ( "zone_id" ),
 CONSTRAINT "ind_72" UNIQUE ( "zone_code" )
);

-- ************************************** "Power Types"

CREATE TABLE IF NOT EXISTS "Power Types"
(
 "power_id"   serial NOT NULL,
 "power_name" varchar(50) NOT NULL,
 CONSTRAINT "PK_power types" PRIMARY KEY ( "power_id" ),
 CONSTRAINT "ind_85" UNIQUE ( "power_name" )
);

-- ************************************** "Groups"

CREATE TABLE IF NOT EXISTS "Groups"
(
 "group_id"          serial NOT NULL,
 "group_name"        varchar(50) NOT NULL,
 "group_description" varchar(250) NULL,
 CONSTRAINT "PK_groups" PRIMARY KEY ( "group_id" ),
 CONSTRAINT "ind_80" UNIQUE ( "group_name" )
);

-- ************************************** "Users"

CREATE TABLE IF NOT EXISTS "Users"
(
 "user_id"     serial NOT NULL,
 "username"    varchar(50) NOT NULL,
 "fullname"    varchar(200) NOT NULL,
 "password"    varchar(128) NOT NULL,
 "email"       varchar(254) NULL,
 "phonenumber" char(12) NULL,
 "is_admin"    boolean NOT NULL DEFAULT FALSE,
 "group_id"    integer NOT NULL,
 CONSTRAINT "PK_users" PRIMARY KEY ( "user_id" ),
 CONSTRAINT "ind_73" UNIQUE ( "username" ),
 CONSTRAINT "FK_55" FOREIGN KEY ( "group_id" ) REFERENCES "Groups" ( "group_id" )
);

CREATE INDEX "fkIdx_55" ON "Users"
(
 "group_id"
);

-- ************************************** "Permissions"

CREATE TABLE IF NOT EXISTS "Permissions"
(
 "group_id"        integer NOT NULL,
 "station_id"      integer NOT NULL,
 "permission_code" smallint NOT NULL,
 CONSTRAINT "PK_permissions" PRIMARY KEY ( "group_id", "station_id" ),
 CONSTRAINT "FK_60" FOREIGN KEY ( "group_id" ) REFERENCES "Groups" ( "group_id" ),
 CONSTRAINT "FK_67" FOREIGN KEY ( "station_id" ) REFERENCES "Stations" ( "station_id" )
);

CREATE INDEX "fkIdx_60" ON "Permissions"
(
 "group_id"
);

CREATE INDEX "fkIdx_67" ON "Permissions"
(
 "station_id"
);

-- ************************************** "Stations"

CREATE TABLE IF NOT EXISTS "Stations"
(
 "station_id"          serial NOT NULL,
 "station_code"        varchar(10) NOT NULL,
 "station_address"     varchar(100) NOT NULL,
 "station_sim"         varchar(12) NOT NULL,
 "station_coordinates" point NOT NULL,
 "station_connection"  boolean NOT NULL,
 "station_display"     boolean NOT NULL,
 "station_ttl"         integer NOT NULL,
 "station_tts"         integer NOT NULL,
 "station_dp"          integer NOT NULL,
 "sensor_thresholds"   json NOT NULL,
 "sensor_parameters"   json NOT NULL,
 "device_parameters"   json NOT NULL,
 "zone_id"             integer NOT NULL,
 "power_id"            integer NOT NULL,
 CONSTRAINT "PK_location" PRIMARY KEY ( "station_id" ),
 CONSTRAINT "ind_74" UNIQUE ( "station_code" ),
 CONSTRAINT "FK_16" FOREIGN KEY ( "zone_id" ) REFERENCES "Zones" ( "zone_id" ),
 CONSTRAINT "FK_92" FOREIGN KEY ( "power_id" ) REFERENCES "Power Types" ( "power_id" )
);

CREATE INDEX "fkIdx_16" ON "Stations"
(
 "zone_id"
);

CREATE INDEX "fkIdx_92" ON "Stations"
(
 "power_id"
);

-- ************************************** "Data"

CREATE TABLE IF NOT EXISTS "data"
(
 "station_id" integer NOT NULL,
 "payload"    json NOT NULL,
 "time"       timestamp with time zone NOT NULL,
 CONSTRAINT "PK_timeseries data" PRIMARY KEY ( "station_id", "time" ),
 CONSTRAINT "FK_24" FOREIGN KEY ( "station_id" ) REFERENCES "Stations" ( "station_id" )
);

CREATE INDEX "fkIdx_24" ON "data"
(
 "station_id"
);

SELECT create_hypertable('data'. 'time');