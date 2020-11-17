
drop database   if exists leagueDB;
create database if not exists leagueDB;

use leagueDB;

select "start create" as "";

create table if not exists Species
(
    ID             int unsigned unique not null ,
    Name           varchar(255)        not null ,
    ScientificName varchar(255)            null ,

    primary key(ID)
);

create table if not exists Entity
(
    ID          int unsigned unique not null ,
    SpeciesID   int unsigned        not null ,
    Name        varchar(255)        not null ,
    Codename    varchar(255)        not null ,

    foreign key (SpeciesID) references Species(ID)
        on delete restrict,

    primary key(ID)
);


create table if not exists Hero
(
    ID         int unsigned unique not null ,
    EntityID   int unsigned        not null ,
    AccessCode int                 not null ,
    Username   varchar(255) unique not null ,
    Password   varchar(255)        not null ,


    foreign key(EntityID) references Entity(ID) on delete restrict,
    primary key(ID)
);

create table if not exists Villain
(
    ID          int unsigned unique not null ,
    EntityID    int unsigned unique not null ,
    RivalHeroID int unsigned        not null ,

    foreign key(EntityID) references Entity(ID) on delete restrict,
    primary key(ID)
);


create table if not exists Hiding
(
    ID        int unsigned unique not null ,
    VillainID int unsigned        not null ,
    Name varchar(255) not null,
    Location varchar(255) not null,
    primary key(ID),
    foreign key(VillainID) references Villain(ID)
);


create table if not exists Equipment
(
    ID          int unsigned unique not null ,
    Name        varchar(255)                 ,
    Description text                         ,
    Utility     varchar(255),
    EntityID    int unsigned        not null ,

    foreign key(EntityID) references Entity(ID),
    primary key(ID)
);


create table if not exists Attack
(
    ID        int unsigned unique not null ,
    EventDate date                not null ,
    Location varchar(255) not null,
    primary key(ID)
);


create table if not exists Attribute
(
    ID       int unsigned unique not null ,
    Name     varchar(255) not null        ,
    IsAdvantage bool                         ,
    primary key(ID)
);

create table if not exists AttackAggregation
(
    VillainID int unsigned not null,
    AttackID  int unsigned not null,
    foreign key(VillainID) references Villain(ID) on delete restrict,
    foreign key(AttackID) references Attack(ID)   on delete restrict
);




create table if not exists AttributeAggregation
(
    AttributeID int unsigned ,
    EntityID    int unsigned ,
    foreign key(AttributeID) references Attribute(ID) on delete restrict,
    foreign key(EntityID) references Entity(ID)       on delete restrict
);

select "create ok" as "";

