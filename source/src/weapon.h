enum
{
    GUN_KNIFE = 0,
    GUN_PISTOL,
    GUN_SHOTGUN,
    GUN_SUBGUN,
    GUN_SNIPER,
    GUN_ASSAULT,
    GUN_GRENADE,
    GUN_AKIMBO,
    GUN_BOLT,
    GUN_HEAL,
    GUN_SWORD,
    GUN_RPG,
    GUN_ASSAULT2,
    GUN_SNIPER2,
    GUN_SNIPER3,
    GUN_PISTOL2,
    NUMGUNS,
    // extra obits
    OBIT_START = NUMGUNS,
    OBIT_DEATH = OBIT_START,
    OBIT_BOT,
    OBIT_SPAWN,
    OBIT_FF,
    OBIT_DROWN,
    OBIT_CHEAT,
    OBIT_FALL_WATER,
    OBIT_FALL,
    OBIT_NUKE,
    OBIT_ASSIST,
    OBIT_TEAM,
    OBIT_SPECT,
    OBIT_SPECIAL,
    OBIT_REVIVE = OBIT_SPECIAL,
    OBIT_JUG,
    OBIT_NUM
};

#define primary_weap(x) (x > 0 && x<NUMGUNS && x != GUN_GRENADE && x != GUN_PISTOL && x!= GUN_HEAL && x != GUN_SWORD && x != GUN_RPG)
#define secondary_weap(x) (x > 0 && x<NUMGUNS && (x == GUN_PISTOL || x == GUN_HEAL || x == GUN_SWORD || x == GUN_RPG || x == GUN_PISTOL2))
#define melee_weap(g) (g == GUN_KNIFE || g == GUN_SWORD)
#define explosive_weap(g) (g == GUN_GRENADE || g == GUN_RPG)
#define suppressed_weap(g) (melee_weap(g) || g == GUN_GRENADE || g == GUN_HEAL)
#define sniper_weap(g) (g == GUN_SNIPER || g == GUN_BOLT || g == GUN_SNIPER2 || g == GUN_SNIPER3)
#define burst_weap(g) (g == GUN_ASSAULT || g == GUN_ASSAULT2 || g == GUN_SUBGUN)
#define ads_gun(g) (!melee_weap(g) && g != GUN_GRENADE && g != GUN_AKIMBO)
#define ads_classic_allowed(g) (!m_classic(gamemode, mutators) || sniper_weap(g) || g == GUN_HEAL)

enum
{
    FRAG_NONE = 0,
    FRAG_GIB = 1 << 0,
    FRAG_SCOPE_NONE = 1 << 1,
    FRAG_SCOPE_FULL = 1 << 2,
    FRAG_REVENGE = 1 << 3,
    FRAG_CRIT = 1 << 4,
    FRAG_FLAG = 1 << 5,
    FRAG_COMEBACK = 1 << 6,
    FRAG_FIRST = 1 << 7,
    FRAG_VALID = (1 << ((7) + 1)) - 1,
    FRAG_SCOPE = FRAG_SCOPE_NONE | FRAG_SCOPE_FULL,
}; // up to 1 << 6 is optimal

enum
{
    STREAK_AIRSTRIKE = 0,
    STREAK_RADAR,
    STREAK_NUKE,
    STREAK_DROPNADE,
    STREAK_REVENGE,
    STREAK_JUG,
    STREAK_NUM
};

class playerent;
class bounceent;

struct weapon
{
    static void equipplayer(playerent *pl);

    weapon(class playerent *owner, int type);
    virtual ~weapon() {}

    int type;
    playerent *owner;
    const struct guninfo &info;
    int &ammo, &mag, &gunwait, shots;
    virtual int dynspread();
    virtual float dynrecoil();
    int reloading, lastaction;

    virtual bool attack(vec &targ) = 0;
    virtual void attackfx(const vec &from, const vec &to, int millis) = 0;
    virtual void attackphysics(const vec &from, const vec &to);
    virtual void attackhit(const vec &o);
    virtual void attacksound();
    virtual bool reload(bool autoreloaded);
    virtual void reset() {}
    virtual bool busy() { return false; }

    virtual int modelanim() = 0;
    virtual void updatetimers(int millis);
    virtual bool selectable();
    virtual bool deselectable();
    virtual void renderstats();
    virtual void renderhudmodel();
    virtual void renderaimhelp(int teamtype);

    virtual void onselecting();
    // virtual void ondeselecting() {}
    virtual void onammopicked() {}
    virtual void onownerdies() {}
    virtual void removebounceent(bounceent *b) {}

    void sendshoot(const vec &to);
    bool modelattacking();
    void renderhudmodel(int lastaction, int index = 0);

    static bool valid(int id);

    virtual int flashtime() const;
};

class grenadeent;

enum { GST_NONE = 0, GST_INHAND, GST_THROWING };

struct grenades : weapon
{
    grenadeent *inhandnade;
    const int throwwait;
    int throwmillis;
    int state;

    grenades(playerent *owner);
    bool attack(vec &targ);
    void attackfx(const vec &from, const vec &to, int millis);
    void attackhit(const vec &o);
    int modelanim();
    void activatenade();
    void thrownade();
    void thrownade(const vec &vel);
    void dropnade();
    void renderstats();
    bool selectable();
    void reset();
    bool busy();
    void onselecting();
    void onownerdies();
    void removebounceent(bounceent *b);
    int flashtime() const;
};


struct gun : weapon
{
    bool autoreloading;
    const int shelltype;

    gun(playerent *owner, int type, int shelltype);
    virtual bool attack(vec &targ);
    void attackshell(const vec &to);
    virtual void attackfx(const vec &from, const vec &to, int millis);
    int modelanim();
    virtual bool reload(bool autoreloaded);
    bool checkautoreload();
};


struct subgun : gun
{
    subgun(playerent *owner) : gun(owner, GUN_SUBGUN, 1) {}
};
struct pistol : gun
{
    pistol(playerent *owner) : gun(owner, GUN_PISTOL, 1) {}
};
struct m1911 : gun
{
    m1911(playerent *owner) : gun(owner, GUN_PISTOL2, 1) {}
};


struct healgun : gun
{
    healgun(playerent *owner);

    void attackfx(const vec &from, const vec &to, int millis);

    int flashtime() const { return 0; }
};


struct sword : weapon
{
    sword(playerent *owner);

    bool attack(vec &targ);
    int modelanim();

    void attackfx(const vec &from, const vec &to, int millis);
    void renderstats(){}

    int flashtime() const;
};


struct crossbow : gun
{
    crossbow(playerent *owner);
    int modelanim();

    virtual void attackfx(const vec &from, const vec &to, int millis);
    void attackhit(const vec &o);
};


struct scopedprimary : gun
{
#define ADSZOOM .90f
    scopedprimary(playerent *owner, int type) : gun(owner, type, 4) {}
    void attackfx(const vec &from, const vec &to, int millis);

    float dynrecoil();
    void renderhudmodel();
    void renderaimhelp(int teamtype);
};
struct m21 : scopedprimary { m21(playerent *owner) : scopedprimary(owner, GUN_SNIPER) {} };
struct m82 : scopedprimary { m82(playerent *owner) : scopedprimary(owner, GUN_SNIPER2) {} };
struct boltrifle : scopedprimary { boltrifle(playerent *owner) : scopedprimary(owner, GUN_BOLT) {} };
struct mk12 : scopedprimary { mk12(playerent *owner) : scopedprimary(owner, GUN_SNIPER3) {} };

struct shotgun : gun
{
    shotgun(playerent *owner);
    int dynspread();
    void attackfx(const vec &from, const vec &to, int millis);
    void renderaimhelp(int teamtype);
};


struct assaultrifle : gun
{
    assaultrifle(playerent *owner, int type) : gun(owner, type, 3) { }
    float dynrecoil();
};
struct m16 : assaultrifle { m16(playerent *owner) : assaultrifle(owner, GUN_ASSAULT) {} };
struct ak47 : assaultrifle { ak47(playerent *owner) : assaultrifle(owner, GUN_ASSAULT2) {} };


struct akimbo : gun
{
    akimbo(playerent *owner);

    int akimboside;
    int akimbomillis;
    int akimbolastaction[2];

    bool attack(vec &targ);
    void onammopicked();
    void onselecting();
    bool selectable();
    void updatetimers(int millis);
    void reset();
    void renderhudmodel();
    bool timerout();
};

class knifeent;

struct knife : weapon
{
    knife(playerent *owner);
    knifeent *inhandknife;
    int state;

    bool attack(vec &targ);
    void reset();
    bool busy();
    bool selectable();
    int modelanim();

    void activateknife();
    void throwknife(bool weak = false);
    void throwknife(const vec &vel);

    void attackfx(const vec &from, const vec &to, int millis);
    void renderstats() {}
    void onownerdies();
    void removebounceent(bounceent *b);

    int flashtime() const;
};

