/* A list of object names in English, from the original objects.c */

/*
 * Rings, unidentified amulets, potions, scrolls, spellbooks, wands, and gems
 * use only the type, material, or color for their descriptions.  For example,
 * a white potion uses only"white" in its string.
 *
 * Each category for which this is true is given a distinct prefix for its
 * name symbol.  For example, the white potion is NAM_POT_WHITE, but the white"NOUN_POT_WHITE, 
 * spellbook is NAM_SPE_WHITE. 
 *
 * This distinction is not important in English, but in many languages an
 * adjective changes form according to the"gender" of the noun it modifies.
 * In Spanish, for example, a white potion is _poci�n blanca_ but a white
 * spellbook is _libro m�gico blanco_.  In the Spanish version of this header,
 * then, NAM_POT_WHITE is"blanca" while NAM_SPE_WHITE is "blanco". 
 */

/* Strange objects */
#define	NAM_STRANGE_OBJECT "NOUN_STRANGE_OBJECT"

/* Weapons, identified */
#define	NAM_ARROW	"NOUN_ARROW"
#define	NAM_ELVEN_ARROW	"NOUN_ELVEN_ARROW"
#define	NAM_ORCISH_ARROW	"NOUN_ORCISH_ARROW"
#define	NAM_SILVER_ARROW	"NOUN_SILVER_ARROW"
#define	NAM_YA	"NOUN_YA"
#define	NAM_CROSSBOW_BOLT	"NOUN_CROSSBOW_BOLT"
#define	NAM_DART	"NOUN_DART"
#define	NAM_SHURIKEN	"NOUN_SHURIKEN"
#define	NAM_BOOMERANG	"NOUN_BOOMERANG"
#define	NAM_SPEAR	"NOUN_SPEAR"
#define	NAM_ELVEN_SPEAR	"NOUN_ELVEN_SPEAR"
#define	NAM_ORCISH_SPEAR	"NOUN_ORCISH_SPEAR"
#define	NAM_DWARVISH_SPEAR	"NOUN_DWARVISH_SPEAR"
#define	NAM_SILVER_SPEAR	"NOUN_SILVER_SPEAR"
#define	NAM_JAVELIN	"NOUN_JAVELIN"
#define	NAM_TRIDENT	"NOUN_TRIDENT"
#define	NAM_DAGGER	"NOUN_DAGGER"
#define	NAM_ELVEN_DAGGER	"NOUN_ELVEN_DAGGER"
#define	NAM_ORCISH_DAGGER	"NOUN_ORCISH_DAGGER"
#define	NAM_SILVER_DAGGER	"NOUN_SILVER_DAGGER"
#define	NAM_ATHAME	"NOUN_ATHAME"
#define	NAM_SCALPEL	"NOUN_SCALPEL"
#define	NAM_KNIFE	"NOUN_KNIFE"
#define	NAM_STILETTO	"NOUN_STILETTO"
#define	NAM_WORM_TOOTH	"NOUN_WORM_TOOTH"
#define	NAM_CRYSKNIFE	"NOUN_CRYSKNIFE"
#define	NAM_AXE	"NOUN_AXE"
#define	NAM_BATTLE_AXE	"NOUN_BATTLE_AXE"
#define	NAM_SHORT_SWORD	"NOUN_SHORT_SWORD"
#define	NAM_ELVEN_SHORT_SWORD	"NOUN_ELVEN_SHORT_SWORD"
#define	NAM_ORCISH_SHORT_SWORD	"NOUN_ORCISH_SHORT_SWORD"
#define	NAM_DWARVISH_SHORT_SWORD	"NOUN_DWARVISH_SHORT_SWORD"
#define	NAM_SCIMITAR	"NOUN_SCIMITAR"
#define	NAM_SILVER_SABER	"NOUN_SILVER_SABER"
#define	NAM_BROADSWORD	"NOUN_BROADSWORD"
#define	NAM_ELVEN_BROADSWORD	"NOUN_ELVEN_BROADSWORD"
#define	NAM_LONG_SWORD	"NOUN_LONG_SWORD"
#define	NAM_TWO_HANDED_SWORD	"NOUN_TWO_HANDED_SWORD"
#define	NAM_KATANA	"NOUN_KATANA"
#define	NAM_TSURUGI	"NOUN_TSURUGI"
#define	NAM_RUNESWORD	"NOUN_RUNESWORD"
#define	NAM_PARTISAN	"NOUN_PARTISAN"
#define	NAM_RANSEUR	"NOUN_RANSEUR"
#define	NAM_SPETUM	"NOUN_SPETUM"
#define	NAM_GLAIVE	"NOUN_GLAIVE"
#define	NAM_LANCE	"NOUN_LANCE"
#define	NAM_HALBERD	"NOUN_HALBERD"
#define	NAM_BARDICHE	"NOUN_BARDICHE"
#define	NAM_VOULGE	"NOUN_VOULGE"
#define	NAM_DWARVISH_MATTOCK	"NOUN_DWARVISH_MATTOCK"
#define	NAM_FAUCHARD	"NOUN_FAUCHARD"
#define	NAM_GUISARME	"NOUN_GUISARME"
#define	NAM_BILL_GUISARME	"NOUN_BILL_GUISARME"
#define	NAM_LUCERN_HAMMER	"NOUN_LUCERN_HAMMER"
#define	NAM_BEC_DE_CORBIN	"NOUN_BEC_DE_CORBIN"
#define	NAM_MACE	"NOUN_MACE"
#define	NAM_MORNING_STAR	"NOUN_MORNING_STAR"
#define	NAM_WAR_HAMMER	"NOUN_WAR_HAMMER"
#define	NAM_CLUB	"NOUN_CLUB"
#define	NAM_RUBBER_HOSE	"NOUN_RUBBER_HOSE"
#define	NAM_QUARTERSTAFF	"NOUN_QUARTERSTAFF"
#define	NAM_AKLYS	"NOUN_AKLYS"
#define	NAM_FLAIL	"NOUN_FLAIL"
#define	NAM_BULLWHIP	"NOUN_BULLWHIP"
#define	NAM_BOW	"NOUN_BOW"
#define	NAM_ELVEN_BOW	"NOUN_ELVEN_BOW"
#define	NAM_ORCISH_BOW	"NOUN_ORCISH_BOW"
#define	NAM_YUMI	"NOUN_YUMI"
#define	NAM_SLING	"NOUN_SLING"
#define	NAM_CROSSBOW	"NOUN_CROSSBOW"

/* Weapons, unidentified */
#define	NAM_RUNED_ARROW	"NOUN_RUNED_ARROW"
#define	NAM_CRUDE_ARROW	"NOUN_CRUDE_ARROW"
#define	NAM_BAMBOO_ARROW	"NOUN_BAMBOO_ARROW"
#define	NAM_THROWING_STAR	"NOUN_THROWING_STAR"
#define	NAM_RUNED_SPEAR	"NOUN_RUNED_SPEAR"
#define	NAM_CRUDE_SPEAR	"NOUN_CRUDE_SPEAR"
#define	NAM_STOUT_SPEAR	"NOUN_STOUT_SPEAR"
#define	NAM_THROWING_SPEAR	"NOUN_THROWING_SPEAR"
#define	NAM_RUNED_DAGGER	"NOUN_RUNED_DAGGER"
#define	NAM_CRUDE_DAGGER	"NOUN_CRUDE_DAGGER"
#define	NAM_DOUBLE_HEADED_AXE	"NOUN_DOUBLE_HEADED_AXE"
#define	NAM_RUNED_SHORT_SWORD	"NOUN_RUNED_SHORT_SWORD"
#define	NAM_CRUDE_SHORT_SWORD	"NOUN_CRUDE_SHORT_SWORD"
#define	NAM_BROAD_SHORT_SWORD	"NOUN_BROAD_SHORT_SWORD"
#define	NAM_CURVED_SWORD	"NOUN_CURVED_SWORD"
#define	NAM_RUNED_BROADSWORD	"NOUN_RUNED_BROADSWORD"
#define	NAM_SAMURAI_SWORD	"NOUN_SAMURAI_SWORD"
#define	NAM_LONG_SAMURAI_SWORD	"NOUN_LONG_SAMURAI_SWORD"
#define	NAM_VULGAR_POLEARM	"NOUN_VULGAR_POLEARM"
#define	NAM_HILTED_POLEARM	"NOUN_HILTED_POLEARM"
#define	NAM_FORKED_POLEARM	"NOUN_FORKED_POLEARM"
#define	NAM_SINGLE_EDGED_POLEARM	"NOUN_SINGLE_EDGED_POLEARM"
#define	NAM_ANGLED_POLEAXE	"NOUN_ANGLED_POLEAXE"
#define	NAM_LONG_POLEAXE	"NOUN_LONG_POLEAXE"
#define	NAM_POLE_CLEAVER	"NOUN_POLE_CLEAVER"
#define	NAM_BROAD_PICK	"NOUN_BROAD_PICK"
#define	NAM_POLE_SICKLE	"NOUN_POLE_SICKLE"
#define	NAM_PRUNING_HOOK	"NOUN_PRUNING_HOOK"
#define	NAM_HOOKED_POLEARM	"NOUN_HOOKED_POLEARM"
#define	NAM_PRONGED_POLEARM	"NOUN_PRONGED_POLEARM"
#define	NAM_BEAKED_POLEARM	"NOUN_BEAKED_POLEARM"
#define	NAM_STAFF	"NOUN_STAFF"
#define	NAM_THONGED_CLUB	"NOUN_THONGED_CLUB"
#define	NAM_RUNED_BOW	"NOUN_RUNED_BOW"
#define	NAM_CRUDE_BOW	"NOUN_CRUDE_BOW"
#define	NAM_LONG_BOW	"NOUN_LONG_BOW"

/* Armor, identified */
#define	NAM_ELVEN_LEATHER_HELM	"NOUN_ELVEN_LEATHER_HELM"
#define	NAM_ORCISH_HELM	"NOUN_ORCISH_HELM"
#define	NAM_DWARVISH_IRON_HELM	"NOUN_DWARVISH_IRON_HELM"
#define	NAM_FEDORA	"NOUN_FEDORA"
#define	NAM_CORNUTHAUM	"NOUN_CORNUTHAUM"
#define	NAM_DUNCE_CAP	"NOUN_DUNCE_CAP"
#define	NAM_DENTED_POT	"NOUN_DENTED_POT"
#define	NAM_HELMET	"NOUN_HELMET"
#define	NAM_HELM_OF_BRILLIANCE	"NOUN_HELM_OF_BRILLIANCE"
#define	NAM_HELM_OF_OPPOSITE_ALIGNMENT	"NOUN_HELM_OF_OPPOSITE_ALIGNMENT"
#define	NAM_HELM_OF_TELEPATHY	"NOUN_HELM_OF_TELEPATHY"
#define	NAM_GRAY_DRAGON_SCALE_MAIL	"NOUN_GRAY_DRAGON_SCALE_MAIL"
#define	NAM_SILVER_DRAGON_SCALE_MAIL	"NOUN_SILVER_DRAGON_SCALE_MAIL"
#define	NAM_SHIMMERING_DRAGON_SCALE_MAIL	"NOUN_SHIMMERING_DRAGON_SCALE_MAIL"
#define	NAM_RED_DRAGON_SCALE_MAIL	"NOUN_RED_DRAGON_SCALE_MAIL"
#define	NAM_WHITE_DRAGON_SCALE_MAIL	"NOUN_WHITE_DRAGON_SCALE_MAIL"
#define	NAM_ORANGE_DRAGON_SCALE_MAIL	"NOUN_ORANGE_DRAGON_SCALE_MAIL"
#define	NAM_BLACK_DRAGON_SCALE_MAIL	"NOUN_BLACK_DRAGON_SCALE_MAIL"
#define	NAM_BLUE_DRAGON_SCALE_MAIL	"NOUN_BLUE_DRAGON_SCALE_MAIL"
#define	NAM_GREEN_DRAGON_SCALE_MAIL	"NOUN_GREEN_DRAGON_SCALE_MAIL"
#define	NAM_YELLOW_DRAGON_SCALE_MAIL	"NOUN_YELLOW_DRAGON_SCALE_MAIL"
#define	NAM_GRAY_DRAGON_SCALES	"NOUN_GRAY_DRAGON_SCALES"
#define	NAM_SILVER_DRAGON_SCALES	"NOUN_SILVER_DRAGON_SCALES"
#define	NAM_SHIMMERING_DRAGON_SCALES	"NOUN_SHIMMERING_DRAGON_SCALES"
#define	NAM_RED_DRAGON_SCALES	"NOUN_RED_DRAGON_SCALES"
#define	NAM_WHITE_DRAGON_SCALES	"NOUN_WHITE_DRAGON_SCALES"
#define	NAM_ORANGE_DRAGON_SCALES	"NOUN_ORANGE_DRAGON_SCALES"
#define	NAM_BLACK_DRAGON_SCALES	"NOUN_BLACK_DRAGON_SCALES"
#define	NAM_BLUE_DRAGON_SCALES	"NOUN_BLUE_DRAGON_SCALES"
#define	NAM_GREEN_DRAGON_SCALES	"NOUN_GREEN_DRAGON_SCALES"
#define	NAM_YELLOW_DRAGON_SCALES	"NOUN_YELLOW_DRAGON_SCALES"
#define	NAM_PLATE_MAIL	"NOUN_PLATE_MAIL"
#define	NAM_CRYSTAL_PLATE_MAIL	"NOUN_CRYSTAL_PLATE_MAIL"
#define	NAM_BRONZE_PLATE_MAIL	"NOUN_BRONZE_PLATE_MAIL"
#define	NAM_SPLINT_MAIL	"NOUN_SPLINT_MAIL"
#define	NAM_BANDED_MAIL	"NOUN_BANDED_MAIL"
#define	NAM_DWARVISH_MITHRIL_COAT	"NOUN_DWARVISH_MITHRIL_COAT"
#define	NAM_ELVEN_MITHRIL_COAT	"NOUN_ELVEN_MITHRIL_COAT"
#define	NAM_CHAIN_MAIL	"NOUN_CHAIN_MAIL"
#define	NAM_ORCISH_CHAIN_MAIL	"NOUN_ORCISH_CHAIN_MAIL"
#define	NAM_SCALE_MAIL	"NOUN_SCALE_MAIL"
#define	NAM_STUDDED_LEATHER_ARMOR	"NOUN_STUDDED_LEATHER_ARMOR"
#define	NAM_RING_MAIL	"NOUN_RING_MAIL"
#define	NAM_ORCISH_RING_MAIL	"NOUN_ORCISH_RING_MAIL"
#define	NAM_LEATHER_ARMOR	"NOUN_LEATHER_ARMOR"
#define	NAM_LEATHER_JACKET	"NOUN_LEATHER_JACKET"
#define	NAM_HAWAIIAN_SHIRT	"NOUN_HAWAIIAN_SHIRT"
#define	NAM_T_SHIRT	"NOUN_T_SHIRT"
#define	NAM_MUMMY_WRAPPING	"NOUN_MUMMY_WRAPPING"
#define	NAM_ELVEN_CLOAK	"NOUN_ELVEN_CLOAK"
#define	NAM_ORCISH_CLOAK	"NOUN_ORCISH_CLOAK"
#define	NAM_DWARVISH_CLOAK	"NOUN_DWARVISH_CLOAK"
#define	NAM_OILSKIN_CLOAK	"NOUN_OILSKIN_CLOAK"
#define	NAM_ROBE	"NOUN_ROBE"
#define	NAM_ALCHEMY_SMOCK	"NOUN_ALCHEMY_SMOCK"
#define NAM_LEATHER_CLOAK	"NOUN_LEATHER_CLOAK"
#define	NAM_CLOAK_OF_PROTECTION	"NOUN_CLOAK_OF_PROTECTION"
#define	NAM_CLOAK_OF_INVISIBILITY	"NOUN_CLOAK_OF_INVISIBILITY"
#define	NAM_CLOAK_OF_MAGIC_RESISTANCE	"NOUN_CLOAK_OF_MAGIC_RESISTANCE"
#define	NAM_CLOAK_OF_DISPLACEMENT	"NOUN_CLOAK_OF_DISPLACEMENT"
#define	NAM_SMALL_SHIELD	"NOUN_SMALL_SHIELD"
#define	NAM_ELVEN_SHIELD	"NOUN_ELVEN_SHIELD"
#define	NAM_URUK_HAI_SHIELD	"NOUN_URUK_HAI_SHIELD"
#define	NAM_ORCISH_SHIELD	"NOUN_ORCISH_SHIELD"
#define	NAM_LARGE_SHIELD	"NOUN_LARGE_SHIELD"
#define	NAM_DWARVISH_ROUNDSHIELD	"NOUN_DWARVISH_ROUNDSHIELD"
#define	NAM_SHIELD_OF_REFLECTION	"NOUN_SHIELD_OF_REFLECTION"
#define	NAM_LEATHER_GLOVES	"NOUN_LEATHER_GLOVES"
#define	NAM_GAUNTLETS_OF_FUMBLING	"NOUN_GAUNTLETS_OF_FUMBLING"
#define	NAM_GAUNTLETS_OF_POWER	"NOUN_GAUNTLETS_OF_POWER"
#define	NAM_GAUNTLETS_OF_DEXTERITY	"NOUN_GAUNTLETS_OF_DEXTERITY"
#define	NAM_LOW_BOOTS	"NOUN_LOW_BOOTS"
#define	NAM_IRON_SHOES	"NOUN_IRON_SHOES"
#define	NAM_HIGH_BOOTS	"NOUN_HIGH_BOOTS"
#define	NAM_SPEED_BOOTS	"NOUN_SPEED_BOOTS"
#define	NAM_WATER_WALKING_BOOTS	"NOUN_WATER_WALKING_BOOTS"
#define	NAM_JUMPING_BOOTS	"NOUN_JUMPING_BOOTS"
#define	NAM_ELVEN_BOOTS	"NOUN_ELVEN_BOOTS"
#define	NAM_KICKING_BOOTS	"NOUN_KICKING_BOOTS"
#define	NAM_FUMBLE_BOOTS	"NOUN_FUMBLE_BOOTS"
#define	NAM_LEVITATION_BOOTS	"NOUN_LEVITATION_BOOTS"

/* Armor, unidentified */
#define	NAM_LEATHER_HAT	"NOUN_LEATHER_HAT"
#define	NAM_IRON_SKULL_CAP	"NOUN_IRON_SKULL_CAP"
#define	NAM_HARD_HAT	"NOUN_HARD_HAT"
#define	NAM_SCHLAPPHUT	"NOUN_SCHLAPPHUT"
#define	NAM_CONICAL_HAT	"NOUN_CONICAL_HAT"
#define	NAM_PLUMED_HELMET	"NOUN_PLUMED_HELMET"
#define	NAM_ETCHED_HELMET	"NOUN_ETCHED_HELMET"
#define	NAM_CRESTED_HELMET	"NOUN_CRESTED_HELMET"
#define	NAM_VISORED_HELMET	"NOUN_VISORED_HELMET"
#define	NAM_CRUDE_CHAIN_MAIL	"NOUN_CRUDE_CHAIN_MAIL"
#define	NAM_CRUDE_RING_MAIL	"NOUN_CRUDE_RING_MAIL"
#define	NAM_FADED_PALL	"NOUN_FADED_PALL"
#define	NAM_COARSE_MANTELET	"NOUN_COARSE_MANTELET"
#define	NAM_HOODED_CLOAK	"NOUN_HOODED_CLOAK"
#define	NAM_SLIPPERY_CLOAK	"NOUN_SLIPPERY_CLOAK"
#define	NAM_APRON	"NOUN_APRON"
#define	NAM_TATTERED_CAPE	"NOUN_TATTERED_CAPE"
#define	NAM_OPERA_CLOAK	"NOUN_OPERA_CLOAK"
#define	NAM_ORNAMENTAL_COPE	"NOUN_ORNAMENTAL_COPE"
#define	NAM_PIECE_OF_CLOTH	"NOUN_PIECE_OF_CLOTH"
#define	NAM_BLUE_AND_GREEN_SHIELD	"NOUN_BLUE_AND_GREEN_SHIELD"
#define	NAM_WHITE_HANDED_SHIELD	"NOUN_WHITE_HANDED_SHIELD"
#define	NAM_RED_EYED_SHIELD	"NOUN_RED_EYED_SHIELD"
#define	NAM_LARGE_ROUND_SHIELD	"NOUN_LARGE_ROUND_SHIELD"
#define	NAM_POLISHED_SILVER_SHIELD	"NOUN_POLISHED_SILVER_SHIELD"
#define	NAM_OLD_GLOVES	"NOUN_OLD_GLOVES"
#define	NAM_PADDED_GLOVES	"NOUN_PADDED_GLOVES"
#define	NAM_RIDING_GLOVES	"NOUN_RIDING_GLOVES"
#define	NAM_FENCING_GLOVES	"NOUN_FENCING_GLOVES"
#define	NAM_WALKING_SHOES	"NOUN_WALKING_SHOES"
#define	NAM_HARD_SHOES	"NOUN_HARD_SHOES"
#define	NAM_JACKBOOTS	"NOUN_JACKBOOTS"
#define	NAM_COMBAT_BOOTS	"NOUN_COMBAT_BOOTS"
#define	NAM_JUNGLE_BOOTS	"NOUN_JUNGLE_BOOTS"
#define	NAM_HIKING_BOOTS	"NOUN_HIKING_BOOTS"
#define	NAM_MUD_BOOTS	"NOUN_MUD_BOOTS"
#define	NAM_BUCKLED_BOOTS	"NOUN_BUCKLED_BOOTS"
#define	NAM_RIDING_BOOTS	"NOUN_RIDING_BOOTS"
#define	NAM_SNOW_BOOTS	"NOUN_SNOW_BOOTS"

/* Rings, identified */
#define	NAM_RING_ADORNMENT	"NOUN_RING_ADORNMENT"
#define	NAM_RING_GAIN_STRENGTH	"NOUN_RING_GAIN_STRENGTH"
#define	NAM_RING_GAIN_CONSTITUTION	"NOUN_RING_GAIN_CONSTITUTION"
#define	NAM_RING_INCREASE_ACCURACY	"NOUN_RING_INCREASE_ACCURACY"
#define	NAM_RING_INCREASE_DAMAGE	"NOUN_RING_INCREASE_DAMAGE"
#define	NAM_RING_PROTECTION	"NOUN_RING_PROTECTION"
#define	NAM_RING_REGENERATION	"NOUN_RING_REGENERATION"
#define	NAM_RING_SEARCHING	"NOUN_RING_SEARCHING"
#define	NAM_RING_STEALTH	"NOUN_RING_STEALTH"
#define	NAM_RING_SUSTAIN_ABILITY	"NOUN_RING_SUSTAIN_ABILITY"
#define	NAM_RING_LEVITATION	"NOUN_RING_LEVITATION"
#define	NAM_RING_HUNGER	"NOUN_RING_HUNGER"
#define	NAM_RING_AGGRAVATE_MONSTER	"NOUN_RING_AGGRAVATE_MONSTER"
#define	NAM_RING_CONFLICT	"NOUN_RING_CONFLICT"
#define	NAM_RING_WARNING	"NOUN_RING_WARNING"
#define	NAM_RING_POISON_RESISTANCE	"NOUN_RING_POISON_RESISTANCE"
#define	NAM_RING_FIRE_RESISTANCE	"NOUN_RING_FIRE_RESISTANCE"
#define	NAM_RING_COLD_RESISTANCE	"NOUN_RING_COLD_RESISTANCE"
#define	NAM_RING_SHOCK_RESISTANCE	"NOUN_RING_SHOCK_RESISTANCE"
#define	NAM_RING_FREE_ACTION	"NOUN_RING_FREE_ACTION"
#define	NAM_RING_SLOW_DIGESTION	"NOUN_RING_SLOW_DIGESTION"
#define	NAM_RING_TELEPORTATION	"NOUN_RING_TELEPORTATION"
#define	NAM_RING_TELEPORT_CONTROL	"NOUN_RING_TELEPORT_CONTROL"
#define	NAM_RING_POLYMORPH	"NOUN_RING_POLYMORPH"
#define	NAM_RING_POLYMORPH_CONTROL	"NOUN_RING_POLYMORPH_CONTROL"
#define	NAM_RING_INVISIBILITY	"NOUN_RING_INVISIBILITY"
#define	NAM_RING_SEE_INVISIBLE	"NOUN_RING_SEE_INVISIBLE"
#define	NAM_RING_PROTECTION_FROM_SHAPE_CHANGERS	"NOUN_RING_PROTECTION_FROM_SHAPE_CHANGERS"

/* Rings, unidentified */
#define	NAM_RING_WOODEN	"RING_UNIDENTIFIED_WOODEN"
#define	NAM_RING_GRANITE "RING_UNIDENTIFIED_GRANITE"
#define	NAM_RING_OPAL	"RING_UNIDENTIFIED_OPAL"
#define	NAM_RING_CLAY	"RING_UNIDENTIFIED_CLAY"
#define	NAM_RING_CORAL	"RING_UNIDENTIFIED_CORAL"
#define	NAM_RING_BLACK_ONYX	"RING_UNIDENTIFIED_BLACK_ONYX"
#define	NAM_RING_MOONSTONE	"RING_UNIDENTIFIED_MOONSTONE"
#define	NAM_RING_TIGER_EYE	"RING_UNIDENTIFIED_TIGER_EYE"
#define	NAM_RING_JADE	"RING_UNIDENTIFIED_JADE"
#define	NAM_RING_BRONZE	"RING_UNIDENTIFIED_BRONZE"
#define	NAM_RING_AGATE	"RING_UNIDENTIFIED_AGATE"
#define	NAM_RING_TOPAZ	"RING_UNIDENTIFIED_TOPAZ"
#define	NAM_RING_SAPPHIRE	"RING_UNIDENTIFIED_SAPPHIRE"
#define	NAM_RING_RUBY	"RING_UNIDENTIFIED_RUBY"
#define	NAM_RING_DIAMOND	"RING_UNIDENTIFIED_DIAMOND"
#define	NAM_RING_PEARL	"RING_UNIDENTIFIED_PEARL"
#define	NAM_RING_IRON	"RING_UNIDENTIFIED_IRON"
#define	NAM_RING_BRASS	"RING_UNIDENTIFIED_BRASS"
#define	NAM_RING_COPPER	"RING_UNIDENTIFIED_COPPER"
#define	NAM_RING_TWISTED	"RING_UNIDENTIFIED_TWISTED"
#define	NAM_RING_STEEL	"RING_UNIDENTIFIED_STEEL"
#define	NAM_RING_SILVER	"RING_UNIDENTIFIED_SILVER"
#define	NAM_RING_GOLD	"RING_UNIDENTIFIED_GOLD"
#define	NAM_RING_IVORY	"RING_UNIDENTIFIED_IVORY"
#define	NAM_RING_EMERALD	"RING_UNIDENTIFIED_EMERALD"
#define	NAM_RING_WIRE	"RING_UNIDENTIFIED_WIRE"
#define	NAM_RING_ENGAGEMENT	"RING_UNIDENTIFIED_ENGAGEMENT"
#define	NAM_RING_SHINY	"RING_UNIDENTIFIED_SHINY"

/* Amulets, identified */
#define	NAM_AMULET_OF_ESP	"NOUN_AMULET_OF_ESP"
#define	NAM_AMULET_OF_LIFE_SAVING	"NOUN_AMULET_OF_LIFE_SAVING"
#define	NAM_AMULET_OF_STRANGULATION	"NOUN_AMULET_OF_STRANGULATION"
#define	NAM_AMULET_OF_RESTFUL_SLEEP	"NOUN_AMULET_OF_RESTFUL_SLEEP"
#define	NAM_AMULET_VERSUS_POISON	"NOUN_AMULET_VERSUS_POISON"
#define	NAM_AMULET_OF_CHANGE	"NOUN_AMULET_OF_CHANGE"
#define	NAM_AMULET_OF_UNCHANGING	"NOUN_AMULET_OF_UNCHANGING"
#define	NAM_AMULET_OF_REFLECTION	"NOUN_AMULET_OF_REFLECTION"
#define	NAM_AMULET_OF_MAGICAL_BREATHING	"NOUN_AMULET_OF_MAGICAL_BREATHING"
#define	NAM_AMULET_OF_YENDOR	"NOUN_AMULET_OF_YENDOR"
#define	NAM_FAKE_AMULET_OF_YENDOR	"NOUN_FAKE_AMULET_OF_YENDOR"

/* Amulets, unidentified */
#define	NAM_AMULET_CIRCULAR	"ADJEKTIV_AMULET_CIRCULAR"
#define	NAM_AMULET_SPHERICAL	"ADJEKTIV_AMULET_SPHERICAL"
#define	NAM_AMULET_OVAL	"ADJEKTIV_AMULET_OVAL"
#define	NAM_AMULET_TRIANGULAR	"ADJEKTIV_AMULET_TRIANGULAR"
#define	NAM_AMULET_PYRAMIDAL	"ADJEKTIV_AMULET_PYRAMIDAL"
#define	NAM_AMULET_SQUARE	"ADJEKTIV_AMULET_SQUARE"
#define	NAM_AMULET_CONCAVE	"ADJEKTIV_AMULET_CONCAVE"
#define	NAM_AMULET_HEXAGONAL	"ADJEKTIV_AMULET_HEXAGONAL"
#define	NAM_AMULET_OCTAGONAL	"ADJEKTIV_AMULET_OCTAGONAL"

/* Tools, identified */
#define	NAM_LARGE_BOX	"NOUN_LARGE_BOX"
#define	NAM_CHEST	"NOUN_CHEST"
#define	NAM_ICE_BOX	"NOUN_ICE_BOX"
#define	NAM_SACK	"NOUN_SACK"
#define	NAM_OILSKIN_SACK	"NOUN_OILSKIN_SACK"
#define	NAM_BAG_OF_HOLDING	"NOUN_BAG_OF_HOLDING"
#define	NAM_BAG_OF_TRICKS	"NOUN_BAG_OF_TRICKS"
#define	NAM_SKELETON_KEY	"NOUN_SKELETON_KEY"
#define	NAM_LOCK_PICK	"NOUN_LOCK_PICK"
#define	NAM_CREDIT_CARD	"NOUN_CREDIT_CARD"
#define	NAM_TALLOW_CANDLE	"NOUN_TALLOW_CANDLE"
#define	NAM_WAX_CANDLE	"NOUN_WAX_CANDLE"
#define	NAM_BRASS_LANTERN	"NOUN_BRASS_LANTERN"
#define	NAM_OIL_LAMP	"NOUN_OIL_LAMP"
#define	NAM_MAGIC_LAMP	"NOUN_MAGIC_LAMP"
#define	NAM_EXPENSIVE_CAMERA	"NOUN_EXPENSIVE_CAMERA"
#define	NAM_MIRROR	"NOUN_MIRROR"
#define	NAM_CRYSTAL_BALL	"NOUN_CRYSTAL_BALL"
#define	NAM_LENSES	"NOUN_LENSES"
#define	NAM_BLINDFOLD	"NOUN_BLINDFOLD"
#define	NAM_TOWEL	"NOUN_TOWEL"
#define	NAM_SADDLE	"NOUN_SADDLE"
#define	NAM_LEASH	"NOUN_LEASH"
#define	NAM_STETHOSCOPE	"NOUN_STETHOSCOPE"
#define	NAM_TINNING_KIT	"NOUN_TINNING_KIT"
#define	NAM_TIN_OPENER	"NOUN_TIN_OPENER"
#define	NAM_CAN_OF_GREASE	"NOUN_CAN_OF_GREASE"
#define	NAM_FIGURINE	"NOUN_FIGURINE"
#define	NAM_MAGIC_MARKER	"NOUN_MAGIC_MARKER"
#define	NAM_LAND_MINE	"NOUN_LAND_MINE"
#define	NAM_BEARTRAP	"NOUN_BEARTRAP"
#define	NAM_TIN_WHISTLE	"NOUN_TIN_WHISTLE"
#define	NAM_MAGIC_WHISTLE	"NOUN_MAGIC_WHISTLE"
#define	NAM_WOODEN_FLUTE	"NOUN_WOODEN_FLUTE"
#define	NAM_MAGIC_FLUTE	"NOUN_MAGIC_FLUTE"
#define	NAM_TOOLED_HORN	"NOUN_TOOLED_HORN"
#define	NAM_FROST_HORN	"NOUN_FROST_HORN"
#define	NAM_FIRE_HORN	"NOUN_FIRE_HORN"
#define	NAM_HORN_OF_PLENTY	"NOUN_HORN_OF_PLENTY"
#define	NAM_WOODEN_HARP	"NOUN_WOODEN_HARP"
#define	NAM_MAGIC_HARP	"NOUN_MAGIC_HARP"
#define	NAM_BELL	"NOUN_BELL"
#define	NAM_BUGLE	"NOUN_BUGLE"
#define	NAM_LEATHER_DRUM	"NOUN_LEATHER_DRUM"
#define	NAM_DRUM_OF_EARTHQUAKE	"NOUN_DRUM_OF_EARTHQUAKE"
#define	NAM_PICK_AXE	"NOUN_PICK_AXE"
#define	NAM_GRAPPLING_HOOK	"NOUN_GRAPPLING_HOOK"
#define	NAM_UNICORN_HORN	"NOUN_UNICORN_HORN"
#define	NAM_CANDELABRUM_OF_INVOCATION	"NOUN_CANDELABRUM_OF_INVOCATION"
#define	NAM_BELL_OF_OPENING	"NOUN_BELL_OF_OPENING"

/* Tools, unidentified */
#define	NAM_BAG	"NOUN_BAG"
#define	NAM_KEY	"NOUN_KEY"
#define	NAM_CANDLE	"NOUN_CANDLE"
#define	NAM_LAMP	"NOUN_LAMP"
#define	NAM_LOOKING_GLASS	"NOUN_LOOKING_GLASS"
#define	NAM_GLASS_ORB	"NOUN_GLASS_ORB"
#define	NAM_WHISTLE	"NOUN_WHISTLE"
#define	NAM_FLUTE	"NOUN_FLUTE"
#define	NAM_HORN	"NOUN_HORN"
#define	NAM_HARP	"NOUN_HARP"
#define	NAM_DRUM	"NOUN_DRUM"
#define	NAM_IRON_HOOK	"NOUN_IRON_HOOK"
#define	NAM_CANDELABRUM	"NOUN_CANDELABRUM"
#define	NAM_SILVER_BELL	"NOUN_SILVER_BELL"

/* Food */
/* Food does not have to be identified, except for tins */
#define	NAM_TRIPE_RATION	"NOUN_TRIPE_RATION"
#define	NAM_CORPSE	"NOUN_CORPSE"
#define	NAM_EGG	"NOUN_EGG"
#define	NAM_MEATBALL	"NOUN_MEATBALL"
#define	NAM_MEAT_STICK	"NOUN_MEAT_STICK"
#define	NAM_HUGE_CHUNK_OF_MEAT	"NOUN_HUGE_CHUNK_OF_MEAT"
#define	NAM_MEAT_RING	"NOUN_MEAT_RING"
#define	NAM_KELP_FROND	"NOUN_KELP_FROND"
#define	NAM_EUCALYPTUS_LEAF	"NOUN_EUCALYPTUS_LEAF"
#define	NAM_APPLE	"NOUN_APPLE"
#define	NAM_ORANGE	"NOUN_ORANGE"
#define	NAM_PEAR	"NOUN_PEAR"
#define	NAM_MELON	"NOUN_MELON"
#define	NAM_BANANA	"NOUN_BANANA"
#define	NAM_CARROT	"NOUN_CARROT"
#define	NAM_SPRIG_OF_WOLFSBANE	"NOUN_SPRIG_OF_WOLFSBANE"
#define	NAM_CLOVE_OF_GARLIC	"NOUN_CLOVE_OF_GARLIC"
#define	NAM_SLIME_MOLD	"NOUN_SLIME_MOLD"
#define	NAM_LUMP_OF_ROYAL_JELLY	"NOUN_LUMP_OF_ROYAL_JELLY"
#define	NAM_CREAM_PIE	"NOUN_CREAM_PIE"
#define	NAM_CANDY_BAR	"NOUN_CANDY_BAR"
#define	NAM_FORTUNE_COOKIE	"NOUN_FORTUNE_COOKIE"
#define	NAM_PANCAKE	"NOUN_PANCAKE"
#define	NAM_LEMBAS_WAFER	"NOUN_LEMBAS_WAFER"
#define	NAM_CRAM_RATION	"NOUN_CRAM_RATION"
#define	NAM_FOOD_RATION	"NOUN_FOOD_RATION"
#define	NAM_K_RATION	"NOUN_K_RATION"
#define	NAM_C_RATION	"NOUN_C_RATION"
#define	NAM_TIN	"NOUN_TIN"

/* Potions, identified */
#define	NAM_POT_GAIN_ABILITY	"NOUN_POT_GAIN_ABILITY"
#define	NAM_POT_RESTORE_ABILITY	"NOUN_POT_RESTORE_ABILITY"
#define	NAM_POT_CONFUSION	"NOUN_POT_CONFUSION"
#define	NAM_POT_BLINDNESS	"NOUN_POT_BLINDNESS"
#define	NAM_POT_PARALYSIS	"NOUN_POT_PARALYSIS"
#define	NAM_POT_SPEED	"NOUN_POT_SPEED"
#define	NAM_POT_LEVITATION	"NOUN_POT_LEVITATION"
#define	NAM_POT_HALLUCINATION	"NOUN_POT_HALLUCINATION"
#define	NAM_POT_INVISIBILITY	"NOUN_POT_INVISIBILITY"
#define	NAM_POT_SEE_INVISIBLE	"NOUN_POT_SEE_INVISIBLE"
#define	NAM_POT_HEALING	"NOUN_POT_HEALING"
#define	NAM_POT_EXTRA_HEALING	"NOUN_POT_EXTRA_HEALING"
#define	NAM_POT_GAIN_LEVEL	"NOUN_POT_GAIN_LEVEL"
#define	NAM_POT_ENLIGHTENMENT	"NOUN_POT_ENLIGHTENMENT"
#define	NAM_POT_MONSTER_DETECTION	"NOUN_POT_MONSTER_DETECTION"
#define	NAM_POT_OBJECT_DETECTION	"NOUN_POT_OBJECT_DETECTION"
#define	NAM_POT_GAIN_ENERGY	"NOUN_POT_GAIN_ENERGY"
#define	NAM_POT_SLEEPING	"NOUN_POT_SLEEPING"
#define	NAM_POT_FULL_HEALING	"NOUN_POT_FULL_HEALING"
#define	NAM_POT_POLYMORPH	"NOUN_POT_POLYMORPH"
#define	NAM_POT_BOOZE	"NOUN_POT_BOOZE"
#define	NAM_POT_SICKNESS	"NOUN_POT_SICKNESS"
#define	NAM_POT_FRUIT_JUICE	"NOUN_POT_FRUIT_JUICE"
#define	NAM_POT_ACID	"NOUN_POT_ACID"
#define	NAM_POT_OIL	"NOUN_POT_OIL"
#define	NAM_POT_WATER	"NOUN_POT_WATER"

/* Potions, unidentified */
#define	NAM_POT_RUBY	"ADJEKTIV_POT_RUBY"
#define	NAM_POT_PINK	"ADJEKTIV_POT_PINK"
#define	NAM_POT_ORANGE	"ADJEKTIV_POT_ORANGE"
#define	NAM_POT_YELLOW	"ADJEKTIV_POT_YELLOW"
#define	NAM_POT_EMERALD	"ADJEKTIV_POT_EMERALD"
#define	NAM_POT_DARK_GREEN	"ADJEKTIV_POT_DARK_GREEN"
#define	NAM_POT_CYAN	"ADJEKTIV_POT_CYAN"
#define	NAM_POT_SKY_BLUE	"ADJEKTIV_POT_SKY_BLUE"
#define	NAM_POT_BRILLIANT_BLUE	"ADJEKTIV_POT_BRILLIANT_BLUE"
#define	NAM_POT_MAGENTA	"ADJEKTIV_POT_MAGENTA"
#define	NAM_POT_PURPLE_RED	"ADJEKTIV_POT_PURPLE_RED"
#define	NAM_POT_PUCE	"ADJEKTIV_POT_PUCE"
#define	NAM_POT_MILKY	"ADJEKTIV_POT_MILKY"
#define	NAM_POT_SWIRLY	"ADJEKTIV_POT_SWIRLY"
#define	NAM_POT_BUBBLY	"ADJEKTIV_POT_BUBBLY"
#define	NAM_POT_SMOKY	"ADJEKTIV_POT_SMOKY"
#define	NAM_POT_CLOUDY	"ADJEKTIV_POT_CLOUDY"
#define	NAM_POT_EFFERVESCENT	"ADJEKTIV_POT_EFFERVESCENT"
#define	NAM_POT_BLACK	"ADJEKTIV_POT_BLACK"
#define	NAM_POT_GOLDEN	"ADJEKTIV_POT_GOLDEN"
#define	NAM_POT_BROWN	"ADJEKTIV_POT_BROWN"
#define	NAM_POT_FIZZY	"ADJEKTIV_POT_FIZZY"
#define	NAM_POT_DARK	"ADJEKTIV_POT_DARK"
#define	NAM_POT_WHITE	"ADJEKTIV_POT_WHITE"
#define	NAM_POT_MURKY	"ADJEKTIV_POT_MURKY"
#define	NAM_POT_CLEAR	"ADJEKTIV_POT_CLEAR"

/* Scrolls, identified */
#define	NAM_SCR_ENCHANT_ARMOR	"NOUN_SCR_ENCHANT_ARMOR"
#define	NAM_SCR_DESTROY_ARMOR	"NOUN_SCR_DESTROY_ARMOR"
#define	NAM_SCR_CONFUSE_MONSTER	"NOUN_SCR_CONFUSE_MONSTER"
#define	NAM_SCR_SCARE_MONSTER	"NOUN_SCR_SCARE_MONSTER"
#define	NAM_SCR_REMOVE_CURSE	"NOUN_SCR_REMOVE_CURSE"
#define	NAM_SCR_ENCHANT_WEAPON	"NOUN_SCR_ENCHANT_WEAPON"
#define	NAM_SCR_CREATE_MONSTER	"NOUN_SCR_CREATE_MONSTER"
#define	NAM_SCR_TAMING	"NOUN_SCR_TAMING"
#define	NAM_SCR_GENOCIDE	"NOUN_SCR_GENOCIDE"
#define	NAM_SCR_LIGHT	"NOUN_SCR_LIGHT"
#define	NAM_SCR_TELEPORTATION	"NOUN_SCR_TELEPORTATION"
#define	NAM_SCR_GOLD_DETECTION	"NOUN_SCR_GOLD_DETECTION"
#define	NAM_SCR_FOOD_DETECTION	"NOUN_SCR_FOOD_DETECTION"
#define	NAM_SCR_IDENTIFY	"NOUN_SCR_IDENTIFY"
#define	NAM_SCR_MAGIC_MAPPING	"NOUN_SCR_MAGIC_MAPPING"
#define	NAM_SCR_AMNESIA	"NOUN_SCR_AMNESIA"
#define	NAM_SCR_FIRE	"NOUN_SCR_FIRE"
#define	NAM_SCR_EARTH	"NOUN_SCR_EARTH"
#define	NAM_SCR_PUNISHMENT	"NOUN_SCR_PUNISHMENT"
#define	NAM_SCR_CHARGING	"NOUN_SCR_CHARGING"
#define	NAM_SCR_STINKING_CLOUD	"NOUN_SCR_STINKING_CLOUD"
#define	NAM_SCR_MAIL	"NOUN_SCR_MAIL"
#define	NAM_SCR_BLANK_PAPER	"NOUN_SCR_BLANK_PAPER"

/* Scrolls, unidentified */
/* It may seem that such names as DAIYEN FOOELS don't need to be translated;
   but there's always the possibility that one or more scroll names may prove
   offensive in some language, and should be changed */
#define	NAM_SCR_ZELGO_MER	"NOUN_SCR_ZELGO_MER"
#define	NAM_SCR_JUYED_AWK_YACC	"NOUN_SCR_JUYED_AWK_YACC"
#define	NAM_SCR_NR_9	"NOUN_SCR_NR_9"
#define	NAM_SCR_XIXAXA_XOXAXA_XUXAXA	"NOUN_SCR_XIXAXA_XOXAXA_XUXAXA"
#define	NAM_SCR_PRATYAVAYAH	"NOUN_SCR_PRATYAVAYAH"
#define	NAM_SCR_DAIYEN_FOOELS	"NOUN_SCR_DAIYEN_FOOELS"
#define	NAM_SCR_LEP_GEX_VEN_ZEA	"NOUN_SCR_LEP_GEX_VEN_ZEA"
#define	NAM_SCR_PRIRUTSENIE	"NOUN_SCR_PRIRUTSENIE"
#define	NAM_SCR_ELBIB_YLOH	"NOUN_SCR_ELBIB_YLOH"
#define	NAM_SCR_VERR_YED_HORRE	"NOUN_SCR_VERR_YED_HORRE"
#define	NAM_SCR_VENZAR_BORGAVVE	"NOUN_SCR_VENZAR_BORGAVVE"
#define	NAM_SCR_THARR	"NOUN_SCR_THARR"
#define	NAM_SCR_YUM_YUM	"NOUN_SCR_YUM_YUM"
#define	NAM_SCR_KERNOD_WEL	"NOUN_SCR_KERNOD_WEL"
#define	NAM_SCR_ELAM_EBOW	"NOUN_SCR_ELAM_EBOW"
#define	NAM_SCR_DUAM_XNAHT	"NOUN_SCR_DUAM_XNAHT"
#define	NAM_SCR_ANDOVA_BEGARIN	"NOUN_SCR_ANDOVA_BEGARIN"
#define	NAM_SCR_KIRJE	"NOUN_SCR_KIRJE"
#define	NAM_SCR_VE_FORBRYDERNE	"NOUN_SCR_VE_FORBRYDERNE"
#define	NAM_SCR_HACKEM_MUCHE	"NOUN_SCR_HACKEM_MUCHE"
#define	NAM_SCR_VELOX_NEB	"NOUN_SCR_VELOX_NEB"
#define	NAM_SCR_FOOBIE_BLETCH	"NOUN_SCR_FOOBIE_BLETCH"
#define	NAM_SCR_TEMOV	"NOUN_SCR_TEMOV"
#define	NAM_SCR_GARVEN_DEH	"NOUN_SCR_GARVEN_DEH"
#define	NAM_SCR_READ_ME	"NOUN_SCR_READ_ME"
#define	NAM_SCR_STAMPED	"ADJEKTIV_SCR_STAMPED"
#define	NAM_SCR_UNLABELED	"ADJEKTIV_SCR_UNLABELED"

/* Spellbooks, identified */
#define	NAM_SPE_DIG	"NOUN_SPE_DIG"
#define	NAM_SPE_MAGIC_MISSILE	"NOUN_SPE_MAGIC_MISSILE"
#define	NAM_SPE_FIREBALL	"NOUN_SPE_FIREBALL"
#define	NAM_SPE_CONE_OF_COLD	"NOUN_SPE_CONE_OF_COLD"
#define	NAM_SPE_SLEEP	"NOUN_SPE_SLEEP"
#define	NAM_SPE_FINGER_OF_DEATH	"NOUN_SPE_FINGER_OF_DEATH"
#define	NAM_SPE_LIGHT	"NOUN_SPE_LIGHT"
#define	NAM_SPE_DETECT_MONSTERS	"NOUN_SPE_DETECT_MONSTERS"
#define	NAM_SPE_HEALING	"NOUN_SPE_HEALING"
#define	NAM_SPE_KNOCK	"NOUN_SPE_KNOCK"
#define	NAM_SPE_FORCE_BOLT	"NOUN_SPE_FORCE_BOLT"
#define	NAM_SPE_CONFUSE_MONSTER	"NOUN_SPE_CONFUSE_MONSTER"
#define	NAM_SPE_CURE_BLINDNESS	"NOUN_SPE_CURE_BLINDNESS"
#define	NAM_SPE_DRAIN_LIFE	"NOUN_SPE_DRAIN_LIFE"
#define	NAM_SPE_SLOW_MONSTER	"NOUN_SPE_SLOW_MONSTER"
#define	NAM_SPE_WIZARD_LOCK	"NOUN_SPE_WIZARD_LOCK"
#define	NAM_SPE_CREATE_MONSTER	"NOUN_SPE_CREATE_MONSTER"
#define	NAM_SPE_DETECT_FOOD	"NOUN_SPE_DETECT_FOOD"
#define	NAM_SPE_CAUSE_FEAR	"NOUN_SPE_CAUSE_FEAR"
#define	NAM_SPE_CLAIRVOYANCE	"NOUN_SPE_CLAIRVOYANCE"
#define	NAM_SPE_CURE_SICKNESS	"NOUN_SPE_CURE_SICKNESS"
#define	NAM_SPE_CHARM_MONSTER	"NOUN_SPE_CHARM_MONSTER"
#define	NAM_SPE_HASTE_SELF	"NOUN_SPE_HASTE_SELF"
#define	NAM_SPE_DETECT_UNSEEN	"NOUN_SPE_DETECT_UNSEEN"
#define	NAM_SPE_LEVITATION	"NOUN_SPE_LEVITATION"
#define	NAM_SPE_EXTRA_HEALING	"NOUN_SPE_EXTRA_HEALING"
#define	NAM_SPE_RESTORE_ABILITY	"NOUN_SPE_RESTORE_ABILITY"
#define	NAM_SPE_INVISIBILITY	"NOUN_SPE_INVISIBILITY"
#define	NAM_SPE_DETECT_TREASURE	"NOUN_SPE_DETECT_TREASURE"
#define	NAM_SPE_REMOVE_CURSE	"NOUN_SPE_REMOVE_CURSE"
#define	NAM_SPE_MAGIC_MAPPING	"NOUN_SPE_MAGIC_MAPPING"
#define	NAM_SPE_IDENTIFY	"NOUN_SPE_IDENTIFY"
#define	NAM_SPE_TURN_UNDEAD	"NOUN_SPE_TURN_UNDEAD"
#define	NAM_SPE_POLYMORPH	"NOUN_SPE_POLYMORPH"
#define	NAM_SPE_TELEPORT_AWAY	"NOUN_SPE_TELEPORT_AWAY"
#define	NAM_SPE_CREATE_FAMILIAR	"NOUN_SPE_CREATE_FAMILIAR"
#define	NAM_SPE_CANCELLATION	"NOUN_SPE_CANCELLATION"
#define	NAM_SPE_PROTECTION	"NOUN_SPE_PROTECTION"
#define	NAM_SPE_JUMPING	"NOUN_SPE_JUMPING"
#define	NAM_SPE_STONE_TO_FLESH	"NOUN_SPE_STONE_TO_FLESH"
#define	NAM_SPE_FLAME_SPHERE	"NOUN_SPE_FLAME_SPHERE"
#define	NAM_SPE_FREEZE_SPHERE	"NOUN_SPE_FREEZE_SPHERE"
#define	NAM_SPE_BLANK_PAPER	"NOUN_SPE_BLANK_PAPER"
#define	NAM_SPE_BOOK_OF_THE_DEAD	"NOUN_SPE_BOOK_OF_THE_DEAD"

/* Spellbooks, unidentified */
#define	NAM_SPE_PARCHMENT	"ADJEKTIV_SPE_PARCHMENT"
#define	NAM_SPE_VELLUM	"ADJEKTIV_SPE_VELLUM"
#define	NAM_SPE_RAGGED	"ADJEKTIV_SPE_RAGGED"
#define	NAM_SPE_DOG_EARED	"ADJEKTIV_SPE_DOG_EARED"
#define	NAM_SPE_MOTTLED	"ADJEKTIV_SPE_MOTTLED"
#define	NAM_SPE_STAINED	"ADJEKTIV_SPE_STAINED"
#define	NAM_SPE_CLOTH	"ADJEKTIV_SPE_CLOTH"
#define	NAM_SPE_LEATHER	"ADJEKTIV_SPE_LEATHER"
#define	NAM_SPE_WHITE	"ADJEKTIV_SPE_WHITE"
#define	NAM_SPE_PINK	"ADJEKTIV_SPE_PINK"
#define	NAM_SPE_RED	"ADJEKTIV_SPE_RED"
#define	NAM_SPE_ORANGE	"ADJEKTIV_SPE_ORANGE"
#define	NAM_SPE_YELLOW	"ADJEKTIV_SPE_YELLOW"
#define	NAM_SPE_VELVET	"ADJEKTIV_SPE_VELVET"
#define	NAM_SPE_LIGHT_GREEN	"ADJEKTIV_SPE_LIGHT_GREEN"
#define	NAM_SPE_DARK_GREEN	"ADJEKTIV_SPE_DARK_GREEN"
#define	NAM_SPE_TURQUOISE	"ADJEKTIV_SPE_TURQUOISE"
#define	NAM_SPE_CYAN	"ADJEKTIV_SPE_CYAN"
#define	NAM_SPE_LIGHT_BLUE	"ADJEKTIV_SPE_LIGHT_BLUE"
#define	NAM_SPE_DARK_BLUE	"ADJEKTIV_SPE_DARK_BLUE"
#define	NAM_SPE_INDIGO	"ADJEKTIV_SPE_INDIGO"
#define	NAM_SPE_MAGENTA	"ADJEKTIV_SPE_MAGENTA"
#define	NAM_SPE_PURPLE	"ADJEKTIV_SPE_PURPLE"
#define	NAM_SPE_VIOLET	"ADJEKTIV_SPE_VIOLET"
#define	NAM_SPE_TAN	"ADJEKTIV_SPE_TAN"
#define	NAM_SPE_PLAID	"ADJEKTIV_SPE_PLAID"
#define	NAM_SPE_LIGHT_BROWN	"ADJEKTIV_SPE_LIGHT_BROWN"
#define	NAM_SPE_DARK_BROWN	"ADJEKTIV_SPE_DARK_BROWN"
#define	NAM_SPE_GRAY	"ADJEKTIV_SPE_GRAY"
#define	NAM_SPE_WRINKLED	"ADJEKTIV_SPE_WRINKLED"
#define	NAM_SPE_DUSTY	"ADJEKTIV_SPE_DUSTY"
#define	NAM_SPE_BRONZE	"ADJEKTIV_SPE_BRONZE"
#define	NAM_SPE_COPPER	"ADJEKTIV_SPE_COPPER"
#define	NAM_SPE_SILVER	"ADJEKTIV_SPE_SILVER"
#define	NAM_SPE_GOLD	"ADJEKTIV_SPE_GOLD"
#define	NAM_SPE_GLITTERING	"ADJEKTIV_SPE_GLITTERING"
#define	NAM_SPE_SHINING	"ADJEKTIV_SPE_SHINING"
#define	NAM_SPE_DULL	"ADJEKTIV_SPE_DULL"
#define	NAM_SPE_THIN	"ADJEKTIV_SPE_THIN"
#define	NAM_SPE_THICK	"ADJEKTIV_SPE_THICK"
#define	NAM_SPE_CANVAS	"ADJEKTIV_SPE_CANVAS"
#define	NAM_SPE_HARDCOVER	"ADJEKTIV_SPE_HARDCOVER"
#define	NAM_SPE_PLAIN	"ADJEKTIV_SPE_PLAIN"
#define	NAM_SPE_PAPYRUS	"ADJEKTIV_SPE_PAPYRUS"

/* Wands, identified */
#define	NAM_WAND_LIGHT	"NOUN_WAND_LIGHT"
#define	NAM_WAND_SECRET_DOOR_DETECTION	"NOUN_WAND_SECRET_DOOR_DETECTION"
#define	NAM_WAND_ENLIGHTENMENT	"NOUN_WAND_ENLIGHTENMENT"
#define	NAM_WAND_CREATE_MONSTER	"NOUN_WAND_CREATE_MONSTER"
#define	NAM_WAND_WISHING	"NOUN_WAND_WISHING"
#define	NAM_WAND_NOTHING	"NOUN_WAND_NOTHING"
#define	NAM_WAND_STRIKING	"NOUN_WAND_STRIKING"
#define	NAM_WAND_MAKE_INVISIBLE	"NOUN_WAND_MAKE_INVISIBLE"
#define	NAM_WAND_SLOW_MONSTER	"NOUN_WAND_SLOW_MONSTER"
#define	NAM_WAND_SPEED_MONSTER	"NOUN_WAND_SPEED_MONSTER"
#define	NAM_WAND_UNDEAD_TURNING	"NOUN_WAND_UNDEAD_TURNING"
#define	NAM_WAND_POLYMORPH	"NOUN_WAND_POLYMORPH"
#define	NAM_WAND_CANCELLATION	"NOUN_WAND_CANCELLATION"
#define	NAM_WAND_TELEPORTATION	"NOUN_WAND_TELEPORTATION"
#define	NAM_WAND_OPENING	"NOUN_WAND_OPENING"
#define	NAM_WAND_LOCKING	"NOUN_WAND_LOCKING"
#define	NAM_WAND_PROBING	"NOUN_WAND_PROBING"
#define	NAM_WAND_DIGGING	"NOUN_WAND_DIGGING"
#define	NAM_WAND_MAGIC_MISSILE	"NOUN_WAND_MAGIC_MISSILE"
#define	NAM_WAND_FIRE	"NOUN_WAND_FIRE"
#define	NAM_WAND_COLD	"NOUN_WAND_COLD"
#define	NAM_WAND_SLEEP	"NOUN_WAND_SLEEP"
#define	NAM_WAND_DEATH	"NOUN_WAND_DEATH"
#define	NAM_WAND_LIGHTNING	"NOUN_WAND_LIGHTNING"

/* Wands, unidentified */
#define	NAM_WAND_GLASS	"ADJEKTIV_WAND_GLASS"
#define	NAM_WAND_BALSA	"MADE_OF_WAND_BALSA"
#define	NAM_WAND_CRYSTAL	"ADJEKTIV_WAND_CRYSTAL"
#define	NAM_WAND_MAPLE	"MADE_OF_WAND_MAPLE"
#define	NAM_WAND_PINE	"MADE_OF_WAND_PINE"
#define	NAM_WAND_OAK	"MADE_OF_WAND_OAK"
#define	NAM_WAND_EBONY	"MADE_OF_WAND_EBONY"
#define	NAM_WAND_MARBLE	"MADE_OF_WAND_MARBLE"
#define	NAM_WAND_TIN	"MADE_OF_WAND_TIN"
#define	NAM_WAND_BRASS	"MADE_OF_WAND_BRASS"
#define	NAM_WAND_COPPER	"ADJEKTIV_WAND_COPPER"
#define	NAM_WAND_SILVER	"ADJEKTIV_WAND_SILVER"
#define	NAM_WAND_PLATINUM	"MADE_OF_WAND_PLATINUM"
#define	NAM_WAND_IRIDIUM	"MADE_OF_WAND_IRIDIUM"
#define	NAM_WAND_ZINC	"MADE_OF_WAND_ZINC"
#define	NAM_WAND_ALUMINUM	"MADE_OF_WAND_ALUMINUM"
#define	NAM_WAND_URANIUM	"MADE_OF_WAND_URANIUM"
#define	NAM_WAND_IRON	"ADJEKTIV_WAND_IRON"
#define	NAM_WAND_STEEL	"ADJEKTIV_WAND_STEEL"
#define	NAM_WAND_HEXAGONAL	"ADJEKTIV_WAND_HEXAGONAL"
#define	NAM_WAND_SHORT	"ADJEKTIV_WAND_SHORT"
#define	NAM_WAND_RUNED	"ADJEKTIV_WAND_RUNED"
#define	NAM_WAND_LONG	"ADJEKTIV_WAND_LONG"
#define	NAM_WAND_CURVED	"ADJEKTIV_WAND_CURVED"
#define	NAM_WAND_FORKED	"ADJEKTIV_WAND_FORKED"
#define	NAM_WAND_SPIKED	"ADJEKTIV_WAND_SPIKED"
#define	NAM_WAND_JEWELED	"ADJEKTIV_WAND_JEWELED"

/* Coins */
#define	NAM_GOLD_PIECE	"NOUN_GOLD_PIECE"

/* Gems, identified */
#define	NAM_GEM_DILITHIUM_CRYSTAL	"NOUN_GEM_DILITHIUM_CRYSTAL"
#define	NAM_GEM_DIAMOND	"NOUN_GEM_DIAMOND"
#define	NAM_GEM_RUBY	"NOUN_GEM_RUBY"
#define	NAM_GEM_JACINTH	"NOUN_GEM_JACINTH"
#define	NAM_GEM_SAPPHIRE	"NOUN_GEM_SAPPHIRE"
#define	NAM_GEM_BLACK_OPAL	"NOUN_GEM_BLACK_OPAL"
#define	NAM_GEM_EMERALD	"NOUN_GEM_EMERALD"
#define	NAM_GEM_TURQUOISE	"NOUN_GEM_TURQUOISE"
#define	NAM_GEM_CITRINE	"NOUN_GEM_CITRINE"
#define	NAM_GEM_AQUAMARINE	"NOUN_GEM_AQUAMARINE"
#define	NAM_GEM_AMBER	"NOUN_GEM_AMBER"
#define	NAM_GEM_TOPAZ	"NOUN_GEM_TOPAZ"
#define	NAM_GEM_JET	"NOUN_GEM_JET"
#define	NAM_GEM_OPAL	"NOUN_GEM_OPAL"
#define	NAM_GEM_CHRYSOBERYL	"NOUN_GEM_CHRYSOBERYL"
#define	NAM_GEM_GARNET	"NOUN_GEM_GARNET"
#define	NAM_GEM_AMETHYST	"NOUN_GEM_AMETHYST"
#define	NAM_GEM_JASPER	"NOUN_GEM_JASPER"
#define	NAM_GEM_FLUORITE	"NOUN_GEM_FLUORITE"
#define	NAM_GEM_OBSIDIAN	"NOUN_GEM_OBSIDIAN"
#define	NAM_GEM_AGATE	"NOUN_GEM_AGATE"
#define	NAM_GEM_JADE	"NOUN_GEM_JADE"
#define	NAM_GEM_WHITE_GLASS	"NOUN_GEM_WHITE_GLASS"
#define	NAM_GEM_BLUE_GLASS	"NOUN_GEM_BLUE_GLASS"
#define	NAM_GEM_RED_GLASS	"NOUN_GEM_RED_GLASS"
#define	NAM_GEM_YELLOWISH_BROWN_GLASS	"NOUN_GEM_YELLOWISH_BROWN_GLASS"
#define	NAM_GEM_ORANGE_GLASS	"NOUN_GEM_ORANGE_GLASS"
#define	NAM_GEM_YELLOW_GLASS	"NOUN_GEM_YELLOW_GLASS"
#define	NAM_GEM_BLACK_GLASS	"NOUN_GEM_BLACK_GLASS"
#define	NAM_GEM_GREEN_GLASS	"NOUN_GEM_GREEN_GLASS"
#define	NAM_GEM_VIOLET_GLASS	"NOUN_GEM_VIOLET_GLASS"
#define	NAM_GEM_LUCKSTONE	"NOUN_GEM_LUCKSTONE"
#define	NAM_GEM_LOADSTONE	"NOUN_GEM_LOADSTONE"
#define	NAM_GEM_TOUCHSTONE	"NOUN_GEM_TOUCHSTONE"
#define	NAM_GEM_FLINT	"NOUN_GEM_FLINT"
#define	NAM_GEM_ROCK	"NOUN_GEM_ROCK"

/* Gems, unidentified */
#define	NAM_GEM_WHITE	"ADJEKTIV_GEM_WHITE"
#define	NAM_GEM_RED	"ADJEKTIV_GEM_RED"
#define	NAM_GEM_ORANGE	"ADJEKTIV_GEM_ORANGE"
#define	NAM_GEM_BLUE	"ADJEKTIV_GEM_BLUE"
#define	NAM_GEM_BLACK	"ADJEKTIV_GEM_BLACK"
#define	NAM_GEM_GREEN	"ADJEKTIV_GEM_GREEN"
#define	NAM_GEM_YELLOW	"ADJEKTIV_GEM_YELLOW"
#define	NAM_GEM_YELLOWISH_BROWN	"ADJEKTIV_GEM_YELLOWISH_BROWN"
#define	NAM_GEM_VIOLET	"ADJEKTIV_GEM_VIOLET"
#define	NAM_GEM_GRAY	"ADJEKTIV_GEM_GRAY"

/* Other objects, identified */
#define	NAM_BOULDER	"NOUN_BOULDER"
#define	NAM_STATUE	"NOUN_STATUE"
#define	NAM_HEAVY_IRON_BALL	"NOUN_HEAVY_IRON_BALL"
#define	NAM_IRON_CHAIN	"NOUN_IRON_CHAIN"
#define	NAM_BLINDING_VENOM	"NOUN_BLINDING_VENOM"

/* Other objects, unidentified */
#define	NAM_SPLASH_OF_VENOM	"NOUN_SPLASH_OF_VENOM"
#define	NAM_ACID_VENOM	"NOUN_ACID_VENOM"
