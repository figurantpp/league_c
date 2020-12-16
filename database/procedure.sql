select 'start procedure' as '';

use leagueDB;

delimiter GO

drop procedure if exists LoginOf;

GO

create procedure LoginOf(in username varchar(255), in password varchar(255))
begin
    select Hero.ID         as 'HeroID',
           AccessCode      as 'AccessCode',
           Username        as 'Username',
           Entity.Codename as 'Codename'

    from Hero
             inner join Entity on Entity.ID = Hero.EntityID
    where Hero.Username = username
      and Hero.Password = password
    limit 1;

end

GO

delimiter ;

create view HeroData as


select Entity.ID,
       Entity.Name,
       Codename,
       Species.Name as 'SpeciesName',
       Species.ScientificName,
       Username,
       AccessCode
from Hero
         inner join Entity on Entity.ID = Hero.EntityID
         inner join Species on Species.ID = Entity.SpeciesID;

create view VillainData as

select Villain.ID,
       VillainEntity.Name,
       VillainEntity.Codename,
       VillainSpecies.Name as 'SpeciesName',
       VillainSpecies.ScientificName as 'SpeciesScientificName',
       RivalHero.Username as 'RivalHero'

from Villain
         inner join Entity VillainEntity on Villain.EntityID = VillainEntity.ID
         inner join Species VillainSpecies on VillainEntity.SpeciesID = VillainSpecies.ID
         inner join Hero RivalHero on Villain.RivalHeroID = RivalHero.ID
         inner join Entity HeroEntity on RivalHero.EntityID = HeroEntity.ID;


create view TraitData as
select *
from Trait;

create view AttackData as
select *
from Attack;

create view EquipmentData as
select *
from Equipment;

create view SpeciesData as
select *
from Species;

create view HidingData as
select *
from Hiding;


select 'procedure ok' as '';
