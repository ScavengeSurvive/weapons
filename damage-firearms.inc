// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_damage_firearms
	#undef _inc_damage_firearms
#endif
// custom include-guard to ensure we don't duplicate
#if defined _damage_firearms_included
	#endinput
#endif
#define _damage_firearms_included


#include <YSI\y_hooks>


static
	Float:dmg_ReturnBleedrate[MAX_PLAYERS],
	Float:dmg_ReturnKnockMult[MAX_PLAYERS];


forward OnPlayerShootPlayer(playerid, targetid, bodypart, Float:bleedrate, Float:knockmult, Float:bulletvelocity, Float:distance);


hook OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart) {
	_firearms_handleDamage(playerid, damagedid, bodypart);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

_firearms_handleDamage(playerid, targetid, bodypart) {
	new
		itemid,
		ItemType:itemType,
		WeaponType:weaponType,
		baseWeapon;

	itemid = GetPlayerItem(playerid);
	itemType = GetItemType(itemid);
	GetItemTypeWeaponType(itemType, weaponType);
	GetWeaponTypeBaseWeapon(weaponType, baseWeapon);

	if(!IsBaseWeaponClipBased(baseWeapon)) {
		return 1;
	}

	if(baseWeapon == WEAPON_DEAGLE) {
		new tick;
		GetPlayerDeltDamageTick(playerid, tick);
		if(GetTickCountDifference(GetTickCount(), tick) < 790) {
			return 1;
		}
	} else {
		new tick;
		GetPlayerDeltDamageTick(playerid, tick);
		if(GetTickCountDifference(GetTickCount(), tick) < 100) {
			return 1;
		}
	}

	new ret = _firearm_doDamage(playerid, targetid, itemid, weaponType, bodypart);
	if(!ret) {
		ShowHitMarker(playerid, baseWeapon);
		return 0;
	}

	return ret;
}

_firearm_doDamage(playerid, targetid, itemid, WeaponType:weaponType, bodypart) {
	new
		Calibre:calibre,
		AmmoType:ammoType,
		Float:bleedrate,
		Float:knockmult = 1.0,
		Float:bulletvelocity,
		Float:px,
		Float:py,
		Float:pz,
		Float:tx,
		Float:ty,
		Float:tz,
		Float:distance,
		Float:degrade;

	GetWeaponTypeCalibre(weaponType, calibre);
	GetWeaponAmmoType(itemid, ammoType);
	GetCalibreBleedRate(calibre, bleedrate);
	GetWeaponTypeMuzzVelocity(weaponType, bulletvelocity);
	distance = GetDistance3D(px, py, pz, tx, ty, tz);

	// Turns the muzzle velocity (initial velocity) into a factor that affects
	// the bullet velocity depending on the distance it has travelled.
	// To put it simply, if the weapon has a high muzzle velocity, the round
	// isn't affected by the distance as much as a weapon with a low muzzle
	// velocity.
	degrade = 1.0 - (bulletvelocity / 11300);

	// Now a graph function, the distance is the 'x' and the degradation rate is
	// the power. This gives a curved line which gradually lowers the velocity
	// as the distance increases.
	bulletvelocity -= floatpower(distance, degrade);

	// Now to combine the bullet velocity with the initial bleed rate of the
	// bullet calibre which results in the final bleed rate for the player.
	bleedrate *= bulletvelocity / 1000.0;

	knockmult *= bulletvelocity / 50.0;

	dbg("weapon", "doing firearm damage",
		_f("bleedrate", bleedrate),
		_f("bulletvelocity", bulletvelocity),
		_f("distance", distance),
		_f("degrade", degrade));

	// Apply bleedrate and knockout multiplier from ammotype
	if(IsValidItem(itemid)) {
		new
			Float:bleedMult,
			Float:knockMult;
		GetAmmoTypeBleedrateMultiplier(ammoType, bleedMult);
		GetAmmoTypeKnockoutMultiplier(ammoType, knockMult);
		bleedrate *= bleedMult;
		knockmult *= knockMult;
	}

	dmg_ReturnBleedrate[targetid] = bleedrate;
	dmg_ReturnKnockMult[targetid] = knockmult;

	if(CallLocalFunction(
		"OnPlayerShootPlayer",
		"dddffff",
		playerid,
		targetid,
		bodypart,
		bleedrate,
		knockmult,
		bulletvelocity,
		distance
	)) {
		return 1;
	}

	if(dmg_ReturnBleedrate[targetid] != bleedrate)
		bleedrate = dmg_ReturnBleedrate[targetid];

	if(dmg_ReturnKnockMult[targetid] != knockmult)
		knockmult = dmg_ReturnKnockMult[targetid];

	PlayerInflictWound(
		playerid,
		targetid,
		E_WOUND_FIREARM,
		bleedrate,
		knockmult,
		bodypart,
		"Firearm"
	);

	return 0;
}

stock DMG_FIREARM_SetBleedRate(targetid, Float:bleedrate)
{
	if(!IsPlayerConnected(targetid))
		return 0;

	dmg_ReturnBleedrate[targetid] = bleedrate;

	return 1;
}

stock DMG_FIREARM_SetKnockMult(targetid, Float:knockmult)
{
	if(!IsPlayerConnected(targetid))
		return 0;

	dmg_ReturnKnockMult[targetid] = knockmult;

	return 1;
}
