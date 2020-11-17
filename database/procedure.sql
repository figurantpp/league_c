

select 'start procedure' as '';

use leagueDB;

delimiter GO

drop procedure if exists LoginOf;

GO

create procedure LoginOf(in username varchar(255), in password varchar(255))
begin
    select 

    Hero.ID as 'HeroID',
    AccessCode as 'AccessCode',
    Username as 'Username',
    Entity.Codename as 'Codename'

    from Hero inner join Entity on Entity.ID = Hero.EntityID where Hero.Username = username and Hero.Password = password
    limit 1;

end

GO


create view HeroData as

select 

    Entity.ID,
    Species.Name,
    Codename,
    Species.Name as 'SpeciesName',
    Species.ScientificName,
    Username,
    AccessCode

from Hero

inner join Entity on
Entity.ID = Hero.EntityID

inner join Species on
Species.ID = Entity.SpeciesID;


GO

delimiter ;

select 'procedure ok' as '';
