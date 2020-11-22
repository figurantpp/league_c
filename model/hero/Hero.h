

#ifndef HERO_H
#define HERO_H




/* database view HeroData selection order */
#define HERO_DATA(x) \
    x(char *, id) \
    x(char *, name) \
    x(char *, codename) \
    x(char *, species_name) \
    x(char *, species_scientific_name) \
    x(char *, username) \
    x(char *, access_level)


#define MEMBER(type, name) type name;

struct HeroData
{
    HERO_DATA(MEMBER)
};

typedef struct HeroData StructHeroData;

#undef MEMBER



#ifndef HERO_SOURCE_FILE
#undef HERO_DATA
#endif

#endif