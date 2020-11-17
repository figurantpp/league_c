
use leagueDB;

select "start insertion" as "";




-- # Species


insert into Species
(ID, Name, ScientificName)
values
(
    1,
    "Human",
    "Homo sapiens"
);

--

insert into Species
(ID, Name, ScientificName)
values
(
    2,
    "Dog",
    "Canis lupus familiaris"
);

--

insert into Species
(ID, Name, ScientificName)
values
(
    3,
    "Kriptonian",
    NULL
);

insert into Species
(ID, Name, ScientificName)
values
(
    4,
    "Martian",
    NULL
);

insert into Species
(ID, Name, ScientificName)
values
(
    5,
    "Amazon",
    NULL
);

insert into Species
(ID, Name, ScientificName)
values
(
    6,
    "Atlantian",
    NULL
);


-- # Hero

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    1,
    1,
    "Bruce Wayne",
    "Batman"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    1,
    1,
    10,
    "batman01",
    "beepboop"
);

--

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    2,
    3,
    "Clark Kent",
    "Superman"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    2,
    2,
    rand(),
    "super01",
    "supsup"
);

--

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    3,
    5,
    "Diana Prince",
    "Wonder Woman"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    3,
    3,
    rand(),
    "wowo01",
    "ebaysucks"
);

--

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    4,
    6,
    "Arthur Curry",
    "Aquaman"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    4,
    4,
    1,
    "aquaman01",
    "mermaids-are-hot"
);

--

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    5,
    1,
    "Alan Scott",
    "Green Lantern"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    5,
    5,
    1,
    "green01",
    "greenee"
);

--

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    6,
    1,
    "Barry Allen",
    "Flash"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    6,
    6,
    1,
    "flash",
    "gottagofast"
);

--

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    7,
    1,
    "Victor Stone",
    "Cyborg"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    7,
    7,
    1,
    "robot01",
    "waffles"
);

--

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    8,
    4,
    "John Jones",
    "Martian Manhunter"
);



insert into Hero
(ID, EntityID, AccessCode, Username, Password)
values
(
    8,
    8,
    4,
    "martian01",
    "meet"
);

--


-- # Villain

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    9,
    1,
    "Jack Napier",
    "Joker"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    1,
    9,
    1
);

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    10,
    1,
    "Lex Luthor",
    "Lex Luthor"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    2,
    10,
    2
);

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    11,
    1,
    "Ares",
    "Ares"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    3,
    11,
    3
);

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    12,
    1,
    "David Hyde",
    "Black Manta"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    4,
    12,
    4
);

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    13,
    1,
    "Vandar Adg",
    "Vandal Savage"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    5,
    13,
    5
);

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    14,
    1,
    "Barry Allen",
    "Savitar"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    6,
    14,
    6
);

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    15,
    1,
    "Sebastian Blood VIII",
    "Brother Blood"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    7,
    15,
    7
);

insert into Entity
(ID, SpeciesID, Name, Codename)
values
(
    16,
    1,
    "Despero",
    "Despero"
);

insert into Villain
(ID, EntityID, RivalHeroID)
values
(
    8,
    16,
    8
);

-- Equipment

insert into Equipment
(ID, EntityID, Name, Description, Utility)
values
(
    1,
    1,
    "Batrang",
    "Powerful tool that aways returns to the user",
    "Attack"
);

-- Hidings

insert into Hiding
(ID, VillainID, Name, Location)
values
(
    1,
    2,
    'Lex Corp. Tower',
    'Gotham City'
);

-- Attacks

insert into Attack
(ID, Location, EventDate)
values
(
    1,
    "Arkham Asylum",
    curdate()
);

insert into AttackAggregation
(VillainID, AttackID)
values
(
    1,
    1
);

-- Traits


insert into Trait
(ID, Name, IsAdvantage)
values
(
    1,
    "Flight",
    true
);


insert into TraitAggregation
(TraitID, EntityID)
values
(
    1,
    2
);

select "insert ok" as "";
