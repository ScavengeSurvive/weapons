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
#include <strlib>
#include <item>
#include <item-array-data>
#include <health>
#include <action-text>
#include <language>
#include <inventory>
#include <container>
#include <inventory-dialog>
#include <container-dialog>


// MAX_ITEM_WEAPON is the maximum amount of WeaponTypes that can be defined
#if !defined MAX_ITEM_WEAPON
	#define MAX_ITEM_WEAPON (WeaponType:64)
#endif

#define INVALID_WEAPON_TYPE (WeaponType:-1)


// additional modifiers that describe weapon behaviour
enum (<<= 1) {
	WEAPON_FLAG_ASSISTED_FIRE_ONCE = 1, // fired once by server fire key event
	WEAPON_FLAG_ASSISTED_FIRE,          // fired repeatedly while key pressed
	WEAPON_FLAG_ONLY_FIRE_AIMED,        // only run a fire event while RMB held
	WEAPON_FLAG_LIQUID_AMMO,            // calibre argument is a liquid type
}

enum E_ITEM_WEAPON_DATA {
	ItemType:itmw_itemType,  // Item type the weapon is associated with
	itmw_baseWeapon,         // The base GTA weapon type
	Calibre:itmw_calibre,            // the bullet calibre
	Float:itmw_muzzVelocity, // the simulated muzzle velocity of rounds
	itmw_magSize,            // magazine size
	itmw_maxReserveMags,     // maximum reserve magazines a player can carry
	itmw_animSet,            // animation set the weapon uses
	itmw_flags,              // bitfield flags from the enumator above
}

// Item array data structure
enum e_WEAPON_ITEM_ARRAY_STRUCT {
	WEAPON_ITEM_ARRAY_CELL_MAG,
	WEAPON_ITEM_ARRAY_CELL_RESERVE,
	WEAPON_ITEM_ARRAY_CELL_AMMOTYPE,
	WEAPON_ITEM_ARRAY_CELL_MODS
}


#include "base-weapons"
#include "ammunition-types"
#include "ammunition-transfer"
#include "damage-firearms"
#include "hitmarker"

#include <YSI\y_hooks>


static
	itmw_Data[MAX_ITEM_WEAPON][E_ITEM_WEAPON_DATA],
	itmw_Total,
	WeaponType:itmw_ItemTypeWeapon[MAX_ITEM_TYPE] = {INVALID_WEAPON_TYPE, ...},
	PlayerText:WeaponAmmoUI[MAX_PLAYERS] = {PlayerText:INVALID_TEXT_DRAW, ...},
	tick_LastReload[MAX_PLAYERS],
	tick_GetWeaponTick[MAX_PLAYERS],
	Timer:itmw_RepeatingFireTimer[MAX_PLAYERS],
	Item:itmw_DropItemID[MAX_PLAYERS] = {INVALID_ITEM_ID, ...},
	Timer:itmw_DropTimer[MAX_PLAYERS];


// -
// API
// -


stock WeaponType:DefineItemTypeWeapon(ItemType:itemType, baseweapon, Calibre:calibre, Float:muzzvelocity, magsize, maxreservemags, animset = -1, flags = 0) {
	SetItemTypeMaxArrayData(itemType, e_WEAPON_ITEM_ARRAY_STRUCT);

	new WeaponType:id = WeaponType:itmw_Total++;

	itmw_Data[id][itmw_itemType] = itemType;
	itmw_Data[id][itmw_baseWeapon] = baseweapon;
	itmw_Data[id][itmw_calibre] = calibre;
	itmw_Data[id][itmw_muzzVelocity] = muzzvelocity;
	itmw_Data[id][itmw_magSize] = magsize;
	itmw_Data[id][itmw_maxReserveMags] = maxreservemags;
	itmw_Data[id][itmw_animSet] = animset;
	itmw_Data[id][itmw_flags] = flags;

	itmw_ItemTypeWeapon[itemType] = id;

	return id;
}

stock bool:IsValidWeaponType(WeaponType:weaponType) {
	return WeaponType:0 <= weaponType < WeaponType:itmw_Total;
}

stock bool:IsItemTypeWeapon(ItemType:itemType) {
	if(!IsValidItemType(itemType)) {
		return false;
	}
	return itmw_ItemTypeWeapon[itemType] != INVALID_WEAPON_TYPE;
}

stock AddAmmoToWeapon(Item:itemid, amount) {
	new AmmoType:ammoType;
	GetWeaponAmmoType(itemid, ammoType);
	if(!IsValidAmmoType(ammoType)) {
		return amount;
	}

	new
		ItemType:itemType,
		WeaponType:weaponType,
		magsize,
		reserveammo,
		maxammo,
		remainder = amount;

	itemType = GetItemType(itemid);
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		return amount;
	}
	maxammo = itmw_Data[itmw_ItemTypeWeapon[itemType]][itmw_maxReserveMags] * magsize;

	GetWeaponTypeMagSize(weaponType, magsize);
	GetWeaponReserve(itemid, reserveammo);

	dbg("weapon", "AddAmmoToWeapon",
		_i("ammoType", _:ammoType),
		_i("magsize", magsize));

	if(maxammo == 0) {
		// the maximum amount of ammo the weapon can hold is nothing
		dbg("weapon", "maxammo == 0",
			_i("amount", amount),
			_i("itemid", _:itemid),
			_i("ammoType", _:ammoType),
			_i("magsize", magsize),
			_i("reserveammo", reserveammo));

		if(amount > magsize) {
			remainder = (reserveammo + amount) - magsize;
			amount = magsize;
		} else {
			remainder = 0;
		}

		dbg("weapon", "updating reserve",
			_i("amount", amount));

		SetWeaponReserve(itemid, amount);
	} else {
		// the maximum amount of ammo the weapon can hold is greater than zero
		if(reserveammo == maxammo) {
			return remainder;
		}

		dbg("weapon", "adding ammo to item",
			_i("amount", amount),
			_i("itemid", _:itemid),
			_i("ammoType", _:ammoType),
			_i("reserveammo", reserveammo),
			_i("maxammo", maxammo));

		if(reserveammo + amount > maxammo) {
			remainder = (reserveammo + amount) - maxammo;
			amount = maxammo - reserveammo;
		} else {
			remainder = 0;
		}

		dbg("weapon", "updating reserve",
			_i("amount", amount));

		SetWeaponReserve(itemid, amount + reserveammo);
	}

	dbg("weapon", "finished updating weapon ammo",
		_i("remainder", remainder));

	return remainder;
}


// itmw_ItemTypeWeapon
stock GetItemTypeWeaponType(ItemType:itemType, &WeaponType:weaponType) {
	if(!IsValidItemType(itemType)) {
		return 1;
	}

	weaponType = itmw_ItemTypeWeapon[itemType];
	if(weaponType == INVALID_WEAPON_TYPE) {
		return 2;
	}

	return 0;
}

// itmw_itemType
stock GetWeaponTypeItemType(WeaponType:weaponType, &ItemType:itemType) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	itemType = itmw_Data[weaponType][itmw_itemType];
	if(IsValidItemType(itemType)) {
		return 2;
	}

	return 0;
}

// itmw_baseWeapon
stock GetWeaponTypeBaseWeapon(WeaponType:weaponType, &base) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	base = itmw_Data[weaponType][itmw_baseWeapon];
	return 0;
}

// itmw_calibre
stock GetWeaponTypeCalibre(WeaponType:weaponType, &Calibre:calibre) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	calibre = itmw_Data[weaponType][itmw_calibre];
	return 0;
}

// itmw_muzzVelocity
stock GetWeaponTypeMuzzVelocity(WeaponType:weaponType, &Float:vel) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	vel = itmw_Data[weaponType][itmw_muzzVelocity];
	return 0;
}

// itmw_magSize
stock GetWeaponTypeMagSize(WeaponType:weaponType, &magsize) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	magsize = itmw_Data[weaponType][itmw_magSize];
	return 0;
}

// itmw_maxReserveMags
stock GetWeaponTypeMaxReserveMags(WeaponType:weaponType, &reserve) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	reserve = itmw_Data[weaponType][itmw_maxReserveMags];
	return 0;
}

// itmw_animSet
stock GetWeaponTypeAnimSet(WeaponType:weaponType, &animset) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	animset = itmw_Data[weaponType][itmw_animSet];
	return 0;
}

// itmw_flags
stock GetWeaponTypeFlags(WeaponType:weaponType, &flags) {
	if(!(WeaponType:0 <= weaponType < WeaponType:itmw_Total)) {
		return 1;
	}

	flags = itmw_Data[weaponType][itmw_flags];
	return 0;
}


// -
// Item array data interface - ItemID
// -


// WEAPON_ITEM_ARRAY_CELL_MAG
stock GetWeaponMagAmmo(Item:weaponItemID, &ammo) {
	if(IsError(GetItemArrayDataAtCell(weaponItemID, ammo, WEAPON_ITEM_ARRAY_CELL_MAG))) {
		Handled();
		return 1;
	}
	return 0;
}

stock SetWeaponMagAmmo(Item:weaponItemID, amount) {
	if(amount == 0) {
		new reserve;
		GetWeaponReserve(weaponItemID, reserve);
		if(reserve == 0) {
			SetWeaponAmmoType(weaponItemID, INVALID_AMMUNITION_TYPE);
		}
	}
	SetItemArrayDataSize(weaponItemID, e_WEAPON_ITEM_ARRAY_STRUCT);
	if(IsError(SetItemArrayDataAtCell(weaponItemID, amount, WEAPON_ITEM_ARRAY_CELL_MAG))) {
		Handled();
		return 1;
	}
	return 0;
}

// WEAPON_ITEM_ARRAY_CELL_RESERVE
stock GetWeaponReserve(Item:weaponItemID, &reserve) {
	if(IsError(GetItemArrayDataAtCell(weaponItemID, reserve, WEAPON_ITEM_ARRAY_CELL_RESERVE))) {
		Handled();
		return 1;
	}
	return 0;
}

stock SetWeaponReserve(Item:weaponItemID, amount) {
	if(amount == 0) {
		new mag;
		GetWeaponMagAmmo(weaponItemID, mag);
		if(mag == 0) {
			SetWeaponAmmoType(weaponItemID, INVALID_AMMUNITION_TYPE);
		}
	}
	SetItemArrayDataSize(weaponItemID, e_WEAPON_ITEM_ARRAY_STRUCT);
	if(IsError(SetItemArrayDataAtCell(weaponItemID, amount, WEAPON_ITEM_ARRAY_CELL_RESERVE))) {
		Handled();
		return 1;
	}
	return 0;
}

// WEAPON_ITEM_ARRAY_CELL_AMMOTYPE
stock GetWeaponAmmoType(Item:weaponItemID, &AmmoType:ammoType) {
	if(IsError(GetItemArrayDataAtCell(weaponItemID, _:ammoType, WEAPON_ITEM_ARRAY_CELL_AMMOTYPE))) {
		Handled();
		return 1;
	}
	return 0;
}

stock SetWeaponAmmoType(Item:weaponItemID, AmmoType:ammoType) {
	SetItemArrayDataSize(weaponItemID, e_WEAPON_ITEM_ARRAY_STRUCT);
	if(IsError(SetItemArrayDataAtCell(weaponItemID, _:ammoType, WEAPON_ITEM_ARRAY_CELL_AMMOTYPE))) {
		Handled();
		return 1;
	}
	return 0;
}


// -
// Item array data interface - PlayerID
// -

stock GetPlayerMagAmmo(playerid, &mag) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}
	return GetWeaponMagAmmo(GetPlayerItem(playerid), mag);
}
stock GetPlayerReserveAmmo(playerid, &reserve) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}
	return GetWeaponReserve(GetPlayerItem(playerid), reserve);
}

stock GetPlayerTotalAmmo(playerid, &ammo) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}

	new
		Item:itemid = GetPlayerItem(playerid),
		mag,
		reserve;

	GetWeaponMagAmmo(itemid, mag);
	GetWeaponReserve(itemid, reserve);

	ammo = mag + reserve;
	return 0;
}

stock GivePlayerAmmo(playerid, amount, &remainder) {
	new Item:itemid = GetPlayerItem(playerid);
	if(!IsValidItem(itemid)) {
		return 1;
	}

	remainder = AddAmmoToWeapon(itemid, amount);
	_weapons_update(playerid);
	_weapons_updateUI(playerid);

	return 0;
}

stock RemovePlayerWeapon(playerid)
{
	dbg("weapon", "[RemovePlayerWeapon]");
	if(!IsPlayerConnected(playerid))
		return 0;

	PlayerTextDrawHide(playerid, WeaponAmmoUI[playerid]);
	ResetPlayerWeapons(playerid);

	return 1;
}


// -
// Internal
// -


hook OnScriptInit() {
	new languageid = InitLanguage("English");
	AddLanguageEntry(languageid, "WEAPNOAMMOL", "There is no ammo loaded in this weapon");
	AddLanguageEntry(languageid, "WEAPAUNLOAD", "Unloaded weapon");
	AddLanguageEntry(languageid, "AMWRONGCALI", "Wrong calibre for weapon");
	AddLanguageEntry(languageid, "AMDIFFATYPE", "A different ammunition type is already loaded in this weapon");
	AddLanguageEntry(languageid, "AMMIXINTINS", "Ammo types can't be mixed in tins");
	AddLanguageEntry(languageid, "AMTRANSWTOW", "Transferred %d rounds from weapon to weapon");
	AddLanguageEntry(languageid, "AMTRANSTTOW", "Transferred %d rounds from ammo tin to weapon");
	AddLanguageEntry(languageid, "AMTRANSWTOT", "Transferred %d rounds from weapon to ammo tin");
	AddLanguageEntry(languageid, "AMTRANSTTOT", "Transferred %d rounds from ammo tin to ammo tin");

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

hook OnItemCreate(Item:itemid) {
	if(IsItemTypeWeapon(GetItemType(itemid))) {
		SetItemArrayDataSize(itemid, e_WEAPON_ITEM_ARRAY_STRUCT);
		SetItemArrayDataAtCell(itemid, 0, WEAPON_ITEM_ARRAY_CELL_MAG);
		SetItemArrayDataAtCell(itemid, 0, WEAPON_ITEM_ARRAY_CELL_RESERVE);
		SetItemArrayDataAtCell(itemid, 0, WEAPON_ITEM_ARRAY_CELL_AMMOTYPE);
		SetItemArrayDataAtCell(itemid, 0, WEAPON_ITEM_ARRAY_CELL_MODS);
	}
	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerGetItem(playerid, itemid) {
	new ret = _weapons_update(playerid);
	if(ret) {
		dbg("weapon", "_weapons_update returned nonzero",
			_i("ret", ret));
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerGivenItem(playerid, targetid, Item:itemid) {
	new WeaponType:weapontype;
	GetItemTypeWeaponType(GetItemType(itemid), weapontype);
	if(weapontype != INVALID_WEAPON_TYPE) {
		dbg("weapon", "player given weapon",
			_i("playerid", playerid),
			_i("targetid", targetid),
			_i("itemid", _:itemid),
			_i("weapon", _:weapontype));

		RemovePlayerWeapon(playerid);
		_weapons_update(targetid);
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerDroppedItem(playerid, Item:itemid) {
	new WeaponType:weapontype;
	GetItemTypeWeaponType(GetItemType(itemid), weapontype);
	if(weapontype != INVALID_WEAPON_TYPE) {
		dbg("weapon", "player dropped weapon",
			_i("playerid", playerid),
			_i("itemid", _:itemid),
			_i("weapon", _:weapontype));

		RemovePlayerWeapon(playerid);
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

_weapons_update(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}

	new
		Item:itemid,
		ItemType:itemType;

	itemid = GetPlayerItem(playerid);

	itemType = GetItemType(itemid);
	if(!IsValidItem(itemid)) {
		return 2;
	}

	new WeaponType:weaponType;
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		return 3;
	}

	if(itmw_Data[itmw_ItemTypeWeapon[itemType]][itmw_calibre] == NO_CALIBRE) {
		GivePlayerWeapon(playerid, itmw_Data[itmw_ItemTypeWeapon[itemType]][itmw_baseWeapon], 99999);
		return 4;
	}

	// Get the item type used as ammo for this weapon item
	new AmmoType:ammoType;
	GetWeaponAmmoType(itemid, ammoType);

	// If it's not a valid ammo type, the gun has no ammo loaded.
	if(ammoType == INVALID_AMMUNITION_TYPE) {
		ResetPlayerWeapons(playerid);
		_weapons_updateUI(playerid);
		ShowActionText(playerid, @L(playerid, "WEAPNOAMMOL", true), 3000);
		return 5;
	}

	new magsize;
	GetWeaponTypeMagSize(weaponType, magsize);
	if(magsize > 0) {
		new mag;
		GetWeaponMagAmmo(itemid, mag);
		if(mag > magsize) {
			SetWeaponMagAmmo(itemid, magsize);
			new reserve;
			GetWeaponReserve(itemid, reserve);
			SetWeaponReserve(itemid, reserve + (mag - magsize));
		}
	} else {
		// dbg("weapon", "ERROR: Item weapon %d uses ammo item %d which has a max ammo of %d.", _:itemType, _:ammoitem, magsize);
	}

	new
		magammo,
		reserveammo;
	
	GetWeaponMagAmmo(itemid, magammo);
	GetWeaponReserve(itemid, reserveammo);

	ResetPlayerWeapons(playerid);

	if(magammo == 0) {
		if(reserveammo > 0) {
			_weapons_reload(playerid);
		}
	} else if(magammo > 0) {
		new base;
		GetWeaponTypeBaseWeapon(weaponType, base);
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
		Item:itemid,
		ItemType:itemType;

	itemid = GetPlayerItem(playerid);
	itemType = GetItemType(itemid);

	if(!IsValidItemType(itemType)) {
		if(GetPlayerWeapon(playerid) > 0) {
			RemovePlayerWeapon(playerid);
		}

		return;
	}

	if(!IsItemTypeWeapon(itemType)) {
		return;
	}

	if(itmw_Data[itmw_ItemTypeWeapon[itemType]][itmw_calibre] == NO_CALIBRE) {
		if(IsBaseWeaponThrowable(itmw_Data[itmw_ItemTypeWeapon[itemType]][itmw_baseWeapon])) {
			if(GetPlayerWeapon(playerid) == 0) {
				if(GetTickCountDifference(GetTickCount(), tick_GetWeaponTick[playerid]) > 1000)
					DestroyItem(itemid);
			}
		}

		return;
	}

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		SetPlayerArmedWeapon(playerid, 0);
		return;
	}

	new mag;
	GetWeaponMagAmmo(itemid, mag);
	if(mag <= 0) {
		return;
	}

	new base;
	GetWeaponTypeBaseWeapon(itmw_ItemTypeWeapon[itemType], base);
	SetPlayerArmedWeapon(playerid, base);

	return;
}

timer _RepeatingFire[1000](playerid)
{
	new
		Item:itemid,
		ItemType:itemType,
		magammo;

	itemid = GetPlayerItem(playerid);
	itemType = GetItemType(itemid);
	GetWeaponMagAmmo(itemid, magammo);

	if(!IsValidItemType(itemType)) {
		stop itmw_RepeatingFireTimer[playerid];
		return;
	}

	if(IsPlayerKnockedOut(playerid)) {
		stop itmw_RepeatingFireTimer[playerid];
		return;
	}

	new WeaponType:weaponType;
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		stop itmw_RepeatingFireTimer[playerid];
		return;
	}

	new flags;
	GetWeaponTypeFlags(weaponType, flags);
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
		GetWeaponTypeMaxReserveMags(weaponType, reserveMags);
		magammo -= reserveMags;
		SetWeaponMagAmmo(itemid, magammo);

		if(magammo <= 0) {
			_weapons_reload(playerid);
		}

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

	dbg("weapon", "_weapons_doShot",
		_i("playerid", playerid),
		_i("weaponid", weaponid));

	new
		Item:itemid,
		ItemType:itemType,
		magammo;

	itemid = GetPlayerItem(playerid);
	itemType = GetItemType(itemid);
	GetWeaponMagAmmo(itemid, magammo);

	if(!IsValidItemType(itemType)) {
		// TODO: CallLocalFunction anticheat
		dbg(
			"weapon",
			"player fired weapon type without having any item equipped.",
			_i("playerid", playerid),
			_i("weaponid", weaponid)
		);
		return 0;
	}

	new WeaponType:weaponType;
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		// TODO: CallLocalFunction anticheat
		dbg(
			"weapon",
			"player fired weapon type while having a non-weapon item equipped.",
			_i("playerid", playerid),
			_i("weaponid", weaponid),
			_i("itemType", _:itemType)
		);
		return 0;
	}

	magammo -= 1;

	SetWeaponMagAmmo(itemid, magammo);

	new flags;
	GetWeaponTypeFlags(weaponType, flags);
	if(magammo == 0 && !(flags & WEAPON_FLAG_ASSISTED_FIRE_ONCE)) {
		_weapons_reload(playerid);
	}

	_weapons_updateUI(playerid);

	return 1;
}

_weapons_reload(playerid) {
	dbg("weapon", "_weapons_reload");
	if(GetTickCountDifference(GetTickCount(), tick_LastReload[playerid]) < 1000) {
		return 1;
	}

	new
		Item:itemid,
		ItemType:itemType,
		WeaponType:weaponType,
		Calibre:calibre;

	itemid = GetPlayerItem(playerid);
	itemType = GetItemType(itemid);
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		return 2;
	}

	GetWeaponTypeCalibre(weaponType, calibre);
	if(calibre == NO_CALIBRE) {
		dbg("weapon", "Weapon has no calibre, cancelling reload");
		return 2;
	}

	new
		magammo,
		reserveammo,
		magsize;

	GetWeaponMagAmmo(itemid, magammo);
	GetWeaponReserve(itemid, reserveammo);
	GetWeaponTypeMagSize(weaponType, magsize);

	if(reserveammo == 0) {
		dbg("weapon", "no reserve ammo left to reload with");

		if(magammo == 0) {
			SetWeaponAmmoType(itemid, INVALID_AMMUNITION_TYPE);
			ResetPlayerWeapons(playerid);
		}

		return 3;
	}

	if(magammo == magsize) {
		dbg("weapon", "Mag ammo is the same as mag size");
		return 4;
	}

	if(magsize <= 0) {
		return 5;
	}

	new base;
	GetWeaponTypeBaseWeapon(weaponType, base);

	ResetPlayerWeapons(playerid);

	if(reserveammo + magammo > magsize) {
		SetWeaponMagAmmo(itemid, magsize);
		SetWeaponReserve(itemid, reserveammo - (magsize - magammo));
	} else {
		SetWeaponMagAmmo(itemid, reserveammo + magammo);
		SetWeaponReserve(itemid, 0);
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
		Item:itemid,
		ItemType:itemType,
		WeaponType:weaponType,
		Calibre:calibre;

	itemid = GetPlayerItem(playerid);
	itemType = GetItemType(itemid);
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		return;
	}

	GetWeaponTypeCalibre(weaponType, calibre);

	if(calibre == NO_CALIBRE) {
		dbg("weapon", "weapon is not clip based");
		PlayerTextDrawHide(playerid, WeaponAmmoUI[playerid]);
		return;
	}

	new
		reserveMags,
		mag,
		reserve,
		str[8];
	
	GetWeaponTypeMaxReserveMags(weaponType, reserveMags);
	GetWeaponMagAmmo(itemid, mag);
	GetWeaponReserve(itemid, reserve);

	if(reserveMags > 0) {
		format(str, 8, "%d/%d", mag, reserve);
	} else {
		format(str, 8, "%d", mag);
	}

	PlayerTextDrawSetString(playerid, WeaponAmmoUI[playerid], str);
	PlayerTextDrawShow(playerid, WeaponAmmoUI[playerid]);

	return;
}

// TODO: implicit via item event, not holster event
// hook OnPlayerHolsteredItem(playerid, Item:itemid) {
// 	if(!IsItemTypeWeapon(GetItemType(itemid))) {
// 		return Y_HOOKS_CONTINUE_RETURN_0;
// 	}

// 	new
// 		heldItemID,
// 		ItemType:itemType,
// 		WeaponType:weaponType,
// 		base;
	
// 	heldItemID = GetPlayerItem(playerid);
// 	itemType = GetItemType(heldItemID);
// 	if(GetItemTypeWeaponType(itemType, weaponType)) {
// 		RemovePlayerWeapon(playerid);
// 		return Y_HOOKS_CONTINUE_RETURN_0;
// 	}

// 	GetWeaponTypeBaseWeapon(weaponType, base);
// 	if(base > 0) {
// 		new mag;
// 		GetWeaponMagAmmo(heldItemID, mag);
// 		if(mag == 0) {
// 			RemovePlayerWeapon(playerid);
// 		}
// 	}

// 	return Y_HOOKS_CONTINUE_RETURN_0;
// }

// hook OnPlayerUnHolsteredItem(playerid, itemid) {
// 	if(IsItemTypeWeapon(GetItemType(itemid))) {
// 		_weapons_update(playerid);
// 	}

// 	return Y_HOOKS_CONTINUE_RETURN_0;
// }

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & 1) {
		if(IsPlayerKnockedOut(playerid)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		if(IsPlayerInAnyVehicle(playerid)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new WeaponType:weapontype;
		GetItemTypeWeaponType(GetItemType(GetPlayerItem(playerid)), weapontype);
		if(weapontype != INVALID_WEAPON_TYPE) {
			_weapons_reload(playerid);
		}
	}

	if(newkeys & KEY_FIRE) {
		new
			Item:itemid,
			ItemType:itemType;

		itemid = GetPlayerItem(playerid);

		itemType = GetItemType(itemid);
		if(!IsValidItemType(itemType)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new WeaponType:weapontype;
		if(GetItemTypeWeaponType(itemType, weapontype)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new base;
		GetWeaponTypeBaseWeapon(weapontype, base);
		if(IsBaseWeaponThrowable(base)) {
			defer _weapons_destroyThrowable(playerid, _:itemid);
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new flags;
		GetWeaponTypeFlags(weapontype, flags);
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
		new
			Item:itemid,
			ItemType:itemType;

		itemid = GetPlayerItem(playerid);

		itemType = GetItemType(itemid);
		if(!IsValidItemType(itemType)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new WeaponType:weapontype;
		if(GetItemTypeWeaponType(itemType, weapontype)) {
			return Y_HOOKS_CONTINUE_RETURN_1;
		}

		new base;
		GetWeaponTypeBaseWeapon(weapontype, base);
		if(base == WEAPON_FLAMETHROWER) {
			stop itmw_RepeatingFireTimer[playerid];
		}
	}

	if(oldkeys & KEY_NO) {
		if(IsValidItem(itmw_DropItemID[playerid])) {
			new mag, reserve;
			GetWeaponMagAmmo(itmw_DropItemID[playerid], mag);
			GetWeaponReserve(itmw_DropItemID[playerid], reserve);

			dbg("weapon", "dropping weapon item",
				_i("itemid", _:itmw_DropItemID[playerid]),
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
	DestroyItem(Item:itemid);
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
		ItemType:itemType,
		WeaponType:weaponType;

	itemType = GetItemType(Item:itemid);
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		return 0;
	}

	new maxMags;
	GetWeaponTypeMaxReserveMags(weaponType, maxMags);
	if(maxMags == 0) {
		return 0;
	}

	if(itmw_DropItemID[playerid] != INVALID_ITEM_ID) {
		return 0;
	}

	new flags;
	GetWeaponTypeFlags(weaponType, flags);
	if(flags & WEAPON_FLAG_LIQUID_AMMO) {
		return 0;
	}

	itmw_DropItemID[playerid] = Item:itemid;
	itmw_DropTimer[playerid] = defer _weapons_doUnload(playerid, itemid);

	return 1;
}

timer _weapons_doUnload[300](playerid, itemid) {
	if(GetPlayerItem(playerid) != Item:itemid) {
		itmw_DropItemID[playerid] = INVALID_ITEM_ID;
		return;
	}

	if(itmw_DropItemID[playerid] != Item:itemid) {
		itmw_DropItemID[playerid] = INVALID_ITEM_ID;
		return;
	}

	new
		AmmoType:ammoType,
		ItemType:ammoTypeItemType,
		Float:x,
		Float:y,
		Float:z,
		Float:r,
		Item:ammoitemid;

	GetWeaponAmmoType(Item:itemid, ammoType);
	GetAmmoTypeItemType(ammoType, ammoTypeItemType);
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, r);

	new mag, reserve;
	GetWeaponMagAmmo(Item:itemid, mag);
	GetWeaponReserve(Item:itemid, reserve);

	dbg("weapon", "unloading weapon ammo",
		_i("mag", mag),
		_i("reserve", reserve));

	ammoitemid = CreateItem(ammoTypeItemType,
		x + (0.5 * floatsin(-r, degrees)),
		y + (0.5 * floatcos(-r, degrees)),
		z - ITEM_FLOOR_OFFSET,
		.world = GetPlayerVirtualWorld(playerid),
		.interior = GetPlayerInterior(playerid));

	SetItemArrayDataAtCell(ammoitemid, mag + reserve, 0);

	SetWeaponMagAmmo(Item:itemid, 0);
	SetWeaponReserve(Item:itemid, 0);
	SetWeaponAmmoType(Item:itemid, INVALID_AMMUNITION_TYPE);
	_weapons_update(playerid);
	itmw_DropItemID[playerid] = INVALID_ITEM_ID;

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, 1, 0, 0, 0, 450);
	ShowActionText(playerid, @L(playerid, "WEAPAUNLOAD", true), 3000);

	return;
}

hook OnItemNameRender(Item:itemid, ItemType:itemType) {
	new WeaponType:weapontype;
	GetItemTypeWeaponType(itemType, weapontype);

	if(weapontype == INVALID_WEAPON_TYPE) {
		return Y_HOOKS_CONTINUE_RETURN_0;
	}

	new exname[MAX_ITEM_TEXT];
	_weapons_generateItemName(itemid, exname);
	SetItemNameExtra(itemid, exname);

	return Y_HOOKS_CONTINUE_RETURN_0;
}

_weapons_generateItemName(Item:itemid, output[], len = sizeof output) {
	new ItemType:itemType = GetItemType(itemid);

	new WeaponType:weaponType;
	if(GetItemTypeWeaponType(itemType, weaponType)) {
		return 1;
	}

	new Calibre:calibre;
	GetWeaponTypeCalibre(weaponType, calibre);
	if(calibre == NO_CALIBRE) {
		return 1;
	}

	new
		flags,
		calibrename[MAX_AMMO_CALIBRE_NAME],
		ammoname[MAX_AMMO_CALIBRE_NAME];
	
	GetWeaponTypeFlags(weaponType, flags);
	if(flags & WEAPON_FLAG_LIQUID_AMMO) {
		calibrename = "Liquid";
	} else {
		GetCalibreName(calibre, calibrename);
	}

	new AmmoType:ammoType;
	GetWeaponAmmoType(itemid, ammoType);
	if(ammoType == INVALID_AMMUNITION_TYPE) {
		ammoname = "Unloaded";
	} else {
		GetAmmoTypeName(ammoType, ammoname);
	}

	new
		mag,
		reserve;

	GetWeaponMagAmmo(itemid, mag);
	GetWeaponReserve(itemid, reserve);

	format(output, len, "%d/%d, %s, %s",
		mag,
		reserve,
		calibrename,
		ammoname);

	return 0;
}
