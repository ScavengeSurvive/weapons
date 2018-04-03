// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_weapons
	#undef _inc_weapons
#endif
// custom include-guard to ensure we don't duplicate
#if defined _weapons_included
	#endinput
#endif
#define _weapons_included


#include <a_samp>
#include <logger>
#include <item>
#include <health>

#include "base-weapons"
#include "weapon-types"
#include "ammunition-types"
// #include "damage-firearms"

#include <YSI\y_hooks>


static
	PlayerText:WeaponAmmoUI[MAX_PLAYERS] = {PlayerText:INVALID_TEXT_DRAW, ...},
	tick_LastReload[MAX_PLAYERS],
	tick_GetWeaponTick[MAX_PLAYERS],
	Timer:itmw_RepeatingFireTimer[MAX_PLAYERS],
	itmw_DropItemID[MAX_PLAYERS] = {INVALID_ITEM_ID, ...},
	Timer:itmw_DropTimer[MAX_PLAYERS];


hook OnScriptInit() {
	new languageid = InitLanguage("English");
	AddLanguageEntry(languageid, "WEAPNOAMMOL", "There is no ammo loaded in this weapon");
	AddLanguageEntry(languageid, "WEAPAUNLOAD", "Unloaded weapon");
}

hook OnPlayerConnect(playerid) {
	WeaponAmmoUI[playerid] = CreatePlayerTextDraw(playerid, 520.000000, 64.000000, "500/500");
	PlayerTextDrawAlignment(playerid, WeaponAmmoUI[playerid], 2);
	PlayerTextDrawBackgroundColor(playerid, WeaponAmmoUI[playerid], 255);
	PlayerTextDrawFont(playerid, WeaponAmmoUI[playerid], 1);
	PlayerTextDrawLetterSize(playerid, WeaponAmmoUI[playerid], 0.210000, 1.000000);
	PlayerTextDrawColor(playerid, WeaponAmmoUI[playerid], -1);
	PlayerTextDrawSetOutline(playerid, WeaponAmmoUI[playerid], 1);
	PlayerTextDrawSetProportional(playerid, WeaponAmmoUI[playerid], 1);
	PlayerTextDrawUseBox(playerid, WeaponAmmoUI[playerid], 1);
	PlayerTextDrawBoxColor(playerid, WeaponAmmoUI[playerid], 255);
	PlayerTextDrawTextSize(playerid, WeaponAmmoUI[playerid], 548.000000, 40.000000);

	itmw_DropItemID[playerid] = INVALID_ITEM_ID;
}


// -
// API
// -


stock GivePlayerAmmo(playerid, amount) {
	new itemid = GetPlayerItem(playerid);
	if(!IsValidItem(itemid)) {
		return 0;
	}

	new remainder = AddAmmoToWeapon(itemid, amount);
	_weapons_update(playerid);
	_weapons_updateUI(playerid);

	return remainder;
}

stock RemovePlayerWeapon(playerid)
{
	dbg("weapons", "[RemovePlayerWeapon]");
	if(!IsPlayerConnected(playerid))
		return 0;

	PlayerTextDrawHide(playerid, WeaponAmmoUI[playerid]);
	ResetPlayerWeapons(playerid);

	return 1;
}


// -
// Internal
// -


_weapons_update(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}

	new
		itemid,
		ItemType:itemtype;

	itemid = GetPlayerItem(playerid);
	itemtype = GetItemType(itemid);

	if(!IsValidItem(itemid)) {
		return 2;
	}

	new WeaponType:weapontype;
	GetItemTypeWeapon(itemtype, weapontype);
	if(IsValidWeaponType(weapontype)) {
		return 3;
	}

	new todo;
	// if(itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_calibre] == NO_CALIBRE) {
	// 	GivePlayerWeapon(playerid, itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_baseWeapon], 99999);
	// 	return 4;
	// }

	// Get the item type used as ammo for this weapon item
	new ItemType:ammoitem;
	GetItemWeaponItemAmmoItem(itemid, ammoitem);

	// If it's not a valid ammo type, the gun has no ammo loaded.
	// if(GetItemTypeAmmoType(ammoitem) == -1) {
	// 	ResetPlayerWeapons(playerid);
	// 	_weapons_updateUI(playerid);
	// 	ShowActionText(playerid, @L(playerid, "WEAPNOAMMOL", true), 3000);
	// 	return 5;
	// }

	new magsize;
	GetItemTypeWeaponMagSize(itemtype, magsize);
	if(magsize > 0) {
		new mag;
		GetItemWeaponItemMagAmmo(itemid, mag);
		if(mag > magsize) {
			SetItemWeaponItemMagAmmo(itemid, magsize);
			new reserve;
			GetItemWeaponItemReserve(itemid, reserve);
			SetItemWeaponItemReserve(itemid, reserve + (mag - magsize));
		}
	} else {
		// dbg("weapons", "ERROR: Item weapon %d uses ammo item %d which has a max ammo of %d.", _:itemtype, _:ammoitem, magsize);
	}

	new
		magammo,
		reserveammo;
	
	GetItemWeaponItemMagAmmo(itemid, magammo);
	GetItemWeaponItemReserve(itemid, reserveammo);

	ResetPlayerWeapons(playerid);

	if(magammo == 0) {
		if(reserveammo > 0) {
			_weapons_reload(playerid);
		}
	} else if(magammo > 0) {
		new base;
		GetItemTypeWeaponBaseWeapon(itemtype, base);
		GivePlayerWeapon(playerid, base, 99999);
	}

	_weapons_updateUI(playerid);

	tick_GetWeaponTick[playerid] = GetTickCount();

	return 1;
}

hook OnPlayerUpdate(playerid) {
	_weapons_updateFast(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

_weapons_updateFast(playerid) {
	new
		itemid,
		ItemType:itemtype;

	itemid = GetPlayerItem(playerid);
	itemtype = GetItemType(itemid);

	if(!IsValidItemType(itemtype)) {
		if(GetPlayerWeapon(playerid) > 0) {
			RemovePlayerWeapon(playerid);
		}

		return;
	}

	if(!IsItemTypeWeapon(itemtype)) {
		return;
	}

	new todo;
	// if(itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_calibre] == NO_CALIBRE) {
	// 	if(IsBaseWeaponThrowable(itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_baseWeapon])) {
	// 		if(GetPlayerWeapon(playerid) == 0) {
	// 			if(GetTickCountDifference(GetTickCount(), tick_GetWeaponTick[playerid]) > 1000)
	// 				DestroyItem(itemid);
	// 		}
	// 	}

	// 	return;
	// }

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		SetPlayerArmedWeapon(playerid, 0);
		return;
	}

	new mag;
	GetItemWeaponItemMagAmmo(itemid, mag);
	if(mag <= 0) {
		return;
	}

	new base;
	GetItemTypeWeaponBaseWeapon(itemtype, base);
	SetPlayerArmedWeapon(playerid, base);

	return;
}

timer _RepeatingFire[1000](playerid)
{
	new
		itemid,
		ItemType:itemtype,
		magammo;

	itemid = GetPlayerItem(playerid);
	itemtype = GetItemType(itemid);
	GetItemWeaponItemMagAmmo(itemid, magammo);

	if(!IsValidItemType(itemtype)) {
		stop itmw_RepeatingFireTimer[playerid];
		return;
	}

	if(!IsItemTypeWeapon(itemtype)) {
		stop itmw_RepeatingFireTimer[playerid];
		return;
	}

	if(IsPlayerKnockedOut(playerid)) {
		stop itmw_RepeatingFireTimer[playerid];
		return;
	}

	new flags;
	GetItemTypeWeaponFlags(itemtype, flags);
	if(!(flags & WEAPON_FLAG_ASSISTED_FIRE)) {
		stop itmw_RepeatingFireTimer[playerid];
		return;
	}

	if(GetTickCountDifference(GetTickCount(), tick_LastReload[playerid]) < 1300) {
		return;
	}

	new k, ud, lr;

	GetPlayerKeys(playerid, k, ud, lr);

	if(k & KEY_FIRE) {
		new reserveMags;
		GetItemTypeWeaponMaxReserveMags(itemtype, reserveMags);
		magammo -= reserveMags;
		SetItemWeaponItemMagAmmo(itemid, magammo);

		if(magammo <= 0)
			_weapons_reload(playerid);

		_weapons_updateUI(playerid);
	} else {
		stop itmw_RepeatingFireTimer[playerid];
	}

	return;
}

hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) {
	if(!_weapons_doShot(playerid, weaponid, hittype, hitid, fX, fY, fZ)) {
		return Y_HOOKS_BREAK_RETURN_0;
	}

	return Y_HOOKS_CONTINUE_RETURN_1;
}


_weapons_doShot(playerid, weaponid, hittype = -1, hitid = -1, Float:fX = 0.0, Float:fY = 0.0, Float:fZ = 0.0) {
	#pragma unused hittype, hitid, fX, fY, fZ

	dbg("weapons", "_weapons_doShot",
		_i("playerid", playerid),
		_i("weaponid", weaponid));

	new
		itemid,
		ItemType:itemtype,
		magammo;

	itemid = GetPlayerItem(playerid);
	itemtype = GetItemType(itemid);
	GetItemWeaponItemMagAmmo(itemid, magammo);

	if(!IsValidItemType(itemtype)) {
		new todo;
		// CallLocalFunction() anticheat
		// ChatMsgAdmins(1, YELLOW, "[TEST] Player %p fired weapon type %d without having any item equipped.", playerid, weaponid);
		// dbg("weapons", "[TMPREPORT] Player %p fired weapon type %d without having any item equipped.", playerid, weaponid);
		return 0;
	}

	if(!IsItemTypeWeapon(itemtype)) {
		new todo;
		// ChatMsgAdmins(1, YELLOW, "[TEST] Player %p fired weapon type %d while having a non-weapon item (%d) equipped.", playerid, weaponid, _:itemtype);
		// dbg("weapons", "[TMPREPORT] Player %p fired weapon type %d while having a non-weapon item (%d) equipped.", playerid, weaponid, _:itemtype);
		return 0;
	}

	magammo -= 1;

	SetItemWeaponItemMagAmmo(itemid, magammo);

	new flags;
	GetItemTypeWeaponFlags(itemtype, flags);
	if(magammo == 0 && !(flags & WEAPON_FLAG_ASSISTED_FIRE_ONCE)) {
		_weapons_reload(playerid);
	}

	_weapons_updateUI(playerid);

	return 1;
}

_weapons_reload(playerid) {
	dbg("weapons", "_weapons_reload");
	if(GetTickCountDifference(GetTickCount(), tick_LastReload[playerid]) < 1000) {
		return 1;
	}

	new
		itemid,
		ItemType:itemtype,
		Calibre:calibre;

	itemid = GetPlayerItem(playerid);
	itemtype = GetItemType(itemid);
	GetItemTypeWeaponCalibre(itemtype, calibre);

	if(calibre == NO_CALIBRE) {
		dbg("weapons", "Weapon has no calibre, cancelling reload");
		return 2;
	}

	new
		magammo,
		reserveammo,
		magsize;

	GetItemWeaponItemMagAmmo(itemid, magammo);
	GetItemWeaponItemReserve(itemid, reserveammo);
	GetItemTypeWeaponMagSize(itemtype, magsize);

	if(reserveammo == 0) {
		dbg("weapons", "no reserve ammo left to reload with");

		if(magammo == 0) {
			SetItemWeaponItemAmmoItem(itemid, INVALID_ITEM_TYPE);
			ResetPlayerWeapons(playerid);
		}

		return 3;
	}

	if(magammo == magsize) {
		dbg("weapons", "Mag ammo is the same as mag size");
		return 4;
	}

	if(magsize <= 0) {
		return 5;
	}

	new base;
	GetItemTypeWeaponBaseWeapon(itemtype, base);

	ResetPlayerWeapons(playerid);

	if(reserveammo + magammo > magsize)
	{
		SetItemWeaponItemMagAmmo(itemid, magsize);
		SetItemWeaponItemReserve(itemid, reserveammo - (magsize - magammo));
	} else {
		SetItemWeaponItemMagAmmo(itemid, reserveammo + magammo);
		SetItemWeaponItemReserve(itemid, 0);
	}

	switch(base) {
		// todo: different reload animations
		default:
			ApplyAnimation(playerid, "COLT45", "COLT45_RELOAD", 2.0, 0, 1, 1, 0, 0);
	}

	_weapons_update(playerid);
	_weapons_updateUI(playerid);

	tick_LastReload[playerid] = GetTickCount();

	return 0;
}

_weapons_updateUI(playerid) {
	new
		itemid,
		ItemType:itemtype,
		Calibre:calibre;

	itemid = GetPlayerItem(playerid);
	itemtype = GetItemType(itemid);
	GetItemTypeWeaponCalibre(itemtype, calibre);

	if(calibre == NO_CALIBRE) {
		dbg("weapons", "weapon is not clip based");
		PlayerTextDrawHide(playerid, WeaponAmmoUI[playerid]);
		return;
	}

	new
		reserveMags,
		mag,
		reserve,
		str[8];
	
	GetItemTypeWeaponMaxReserveMags(itemtype, reserveMags);
	GetItemWeaponItemMagAmmo(itemid, mag);
	GetItemWeaponItemReserve(itemid, reserve);

	if(reserveMags > 0) {
		format(str, 8, "%d/%d", mag, reserve);
	} else {
		format(str, 8, "%d", mag);
	}

	PlayerTextDrawSetString(playerid, WeaponAmmoUI[playerid], str);
	PlayerTextDrawShow(playerid, WeaponAmmoUI[playerid]);

	return;
}

hook OnPlayerHolsteredItem(playerid, itemid) {
	if(!IsItemTypeWeapon(GetItemType(itemid))) {
		return Y_HOOKS_CONTINUE_RETURN_0;
	}

	new
		helditemid = GetPlayerItem(playerid),
		base;
	
	GetItemTypeWeaponBaseWeapon(GetItemType(helditemid), base);

	if(base > 0) {
		new mag;
		GetItemWeaponItemMagAmmo(helditemid, mag);
		if(mag == 0) {
			RemovePlayerWeapon(playerid);
		}
	} else {
		RemovePlayerWeapon(playerid);
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerUnHolsteredItem(playerid, itemid) {
	if(IsItemTypeWeapon(GetItemType(itemid))) {
		_weapons_update(playerid);
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & 1) {
		if(IsPlayerKnockedOut(playerid)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		if(IsPlayerInAnyVehicle(playerid)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new WeaponType:weapontype;
		GetItemTypeWeapon(GetItemType(GetPlayerItem(playerid)), weapontype);
		if(weapontype != INVALID_WEAPON_TYPE) {
			_weapons_reload(playerid);
		}
	}

	if(newkeys & KEY_FIRE) {
		new
			itemid,
			ItemType:itemtype;

		itemid = GetPlayerItem(playerid);
		itemtype = GetItemType(itemid);

		if(!IsValidItemType(itemtype)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new WeaponType:weapontype;
		GetItemTypeWeapon(itemtype, weapontype);
		if(weapontype == INVALID_WEAPON_TYPE) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new base;
		GetItemTypeWeaponBaseWeapon(itemtype, base);
		if(IsBaseWeaponThrowable(base)) {
			defer _weapons_destroyThrowable(playerid, itemid);
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new flags;
		GetItemTypeWeaponFlags(itemtype, flags);

		if(flags & WEAPON_FLAG_ONLY_FIRE_AIMED) {
			if(!(newkeys & KEY_HANDBRAKE)) {
				return Y_HOOKS_CONTINUE_RETURN_1;
			}
		}

		if(flags & WEAPON_FLAG_ASSISTED_FIRE_ONCE) {
			_weapons_doShot(playerid, WEAPON_ROCKETLAUNCHER);
		} else if(flags & WEAPON_FLAG_ASSISTED_FIRE) {
			itmw_RepeatingFireTimer[playerid] = repeat _RepeatingFire(playerid);
		}
	}

	if(oldkeys & KEY_FIRE) {
		new base;
		GetItemTypeWeaponBaseWeapon(GetItemType(GetPlayerItem(playerid)), base);
		if(base == WEAPON_FLAMETHROWER) {
			stop itmw_RepeatingFireTimer[playerid];
		}
	}

	if(oldkeys & KEY_NO) {
		if(IsValidItem(itmw_DropItemID[playerid])) {
			new mag, reserve;
			GetItemWeaponItemMagAmmo(itmw_DropItemID[playerid], mag);
			GetItemWeaponItemReserve(itmw_DropItemID[playerid], reserve);

			dbg("weapons", "dropping weapon item",
				_i("itemid", itmw_DropItemID[playerid]),
				_i("mag", mag),
				_i("reserve", reserve));

			stop itmw_DropTimer[playerid];
			PlayerDropItem(playerid);
			itmw_DropItemID[playerid] = INVALID_ITEM_ID;
		}
	}

	return Y_HOOKS_CONTINUE_RETURN_1;
}

timer _weapons_destroyThrowable[1000](playerid, itemid) {
	DestroyItem(itemid);
	ResetPlayerWeapons(playerid);
}

hook OnPlayerDropItem(playerid, itemid) {
	if(_weapons_handleDrop(playerid, itemid)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

_weapons_handleDrop(playerid, itemid) {
	new
		ItemType:itemtype,
		WeaponType:weapontype;

	itemtype = GetItemType(itemid);
	GetItemTypeWeapon(itemtype, weapontype);

	if(weapontype == INVALID_WEAPON_TYPE) {
		return 0;
	}

	new maxMags;
	GetItemTypeWeaponMaxReserveMags(itemtype, maxMags);
	if(maxMags == 0) {
		return 0;
	}

	if(itmw_DropItemID[playerid] != INVALID_ITEM_ID) {
		return 0;
	}

	new flags;
	GetItemTypeWeaponFlags(itemtype, flags);
	if(flags & WEAPON_FLAG_LIQUID_AMMO) {
		return 0;
	}

	itmw_DropItemID[playerid] = itemid;
	itmw_DropTimer[playerid] = defer _weapons_doUnload(playerid, itemid);

	return 1;
}

timer _weapons_doUnload[300](playerid, itemid) {
	if(GetPlayerItem(playerid) != itemid) {
		itmw_DropItemID[playerid] = INVALID_ITEM_ID;
		return;
	}

	if(itmw_DropItemID[playerid] != itemid) {
		itmw_DropItemID[playerid] = INVALID_ITEM_ID;
		return;
	}

	new
		ItemType:ammoitemtype,
		Float:x,
		Float:y,
		Float:z,
		Float:r,
		ammoitemid;

	GetItemWeaponItemAmmoItem(itemid, ammoitemtype);
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, r);

	new mag, reserve;
	GetItemWeaponItemMagAmmo(itemid, mag);
	GetItemWeaponItemReserve(itemid, reserve);

	dbg("weapons", "unloading weapon ammo",
		_i("mag", mag),
		_i("reserve", reserve));

	ammoitemid = CreateItem(ammoitemtype,
		x + (0.5 * floatsin(-r, degrees)),
		y + (0.5 * floatcos(-r, degrees)),
		z - ITEM_FLOOR_OFFSET,
		.world = GetPlayerVirtualWorld(playerid),
		.interior = GetPlayerInterior(playerid));

	SetItemExtraData(ammoitemid, mag + reserve);

	SetItemWeaponItemMagAmmo(itemid, 0);
	SetItemWeaponItemReserve(itemid, 0);
	SetItemWeaponItemAmmoItem(itemid, INVALID_ITEM_TYPE);
	_weapons_update(playerid);
	itmw_DropItemID[playerid] = INVALID_ITEM_ID;

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, 1, 0, 0, 0, 450);
	ShowActionText(playerid, @L(playerid, "WEAPAUNLOAD", true), 3000);

	return;
}

hook OnItemNameRender(itemid, ItemType:itemtype) {
	new WeaponType:weapontype;
	GetItemTypeWeapon(itemtype, weapontype);

	if(weapontype == INVALID_WEAPON_TYPE) {
		return Y_HOOKS_CONTINUE_RETURN_0;
	}

	new Calibre:calibre;
	GetItemTypeWeaponCalibre(itemtype, calibre);
	if(calibre == NO_CALIBRE) {
		return Y_HOOKS_CONTINUE_RETURN_0;
	}

	dbg("weapons", "rendering name for weapon item",
		_i("itemid", itemid),
		_i("itemtype", _:itemtype),
		_i("weapontype", _:weapontype));

	new
		ItemType:ammoitem,
		AmmoType:ammotype,
		flags,
		calibrename[MAX_AMMO_CALIBRE_NAME],
		ammoname[MAX_AMMO_CALIBRE_NAME],
		exname[MAX_ITEM_TEXT];
	
	GetItemWeaponItemAmmoItem(itemid, ammoitem);
	GetItemTypeAmmoType(ammoitem, ammotype);
	GetItemTypeWeaponFlags(itemtype, flags);

	if(flags & WEAPON_FLAG_LIQUID_AMMO) {
		calibrename = "Liquid";
	} else {
		GetCalibreName(calibre, calibrename);
	}

	if(ammotype == INVALID_AMMUNITION_TYPE) {
		ammoname = "Unloaded";
	} else {
		GetAmmoTypeName(ammotype, ammoname);
	}

	new
		name[MAX_ITEM_NAME],
		mag,
		reserve;

	GetItemTypeName(itemtype, name);
	GetItemWeaponItemMagAmmo(itemid, mag);
	GetItemWeaponItemReserve(itemid, reserve);

	format(exname, sizeof(exname), "%d/%d, %s, %s",
		mag,
		reserve,
		calibrename,
		ammoname);
	SetItemNameExtra(itemid, exname);

	return Y_HOOKS_CONTINUE_RETURN_0;
}
