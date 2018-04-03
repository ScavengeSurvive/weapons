// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_weapon_types
	#undef _inc_weapon_types
#endif
// custom include-guard to ensure we don't duplicate
#if defined _weapon_types_included
	#endinput
#endif
#define _weapon_types_included


#include <a_samp>
#include <logger>
#include <item>
#include <item-array-data>
#include <action-text>
#include <language>

#include <YSI\y_hooks>


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
enum {
	WEAPON_ITEM_ARRAY_CELL_MAG,
	WEAPON_ITEM_ARRAY_CELL_RESERVE,
	WEAPON_ITEM_ARRAY_CELL_AMMOITEM,
	WEAPON_ITEM_ARRAY_CELL_MODS
}


static
	itmw_Data[MAX_ITEM_WEAPON][E_ITEM_WEAPON_DATA],
	itmw_Total,
	WeaponType:itmw_ItemTypeWeapon[MAX_ITEM_TYPE] = {INVALID_WEAPON_TYPE, ...};


// -
// API
// -


stock WeaponType:DefineItemTypeWeapon(ItemType:itemtype, baseweapon, Calibre:calibre, Float:muzzvelocity, magsize, maxreservemags, animset = -1, flags = 0) {
	SetItemTypeMaxArrayData(itemtype, 4);

	new WeaponType:id = WeaponType:itmw_Total++;

	itmw_Data[id][itmw_itemType] = itemtype;
	itmw_Data[id][itmw_baseWeapon] = baseweapon;
	itmw_Data[id][itmw_calibre] = calibre;
	itmw_Data[id][itmw_muzzVelocity] = muzzvelocity;
	itmw_Data[id][itmw_magSize] = magsize;
	itmw_Data[id][itmw_maxReserveMags] = maxreservemags;
	itmw_Data[id][itmw_animSet] = animset;
	itmw_Data[id][itmw_flags] = flags;

	itmw_ItemTypeWeapon[itemtype] = id;

	return id;
}

stock bool:IsValidWeaponType(WeaponType:weapontype) {
	return WeaponType:0 <= weapontype < WeaponType:itmw_Total;
}

stock bool:IsItemTypeWeapon(ItemType:itemtype) {
	if(!IsValidItemType(itemtype)) {
		return false;
	}
	return itmw_ItemTypeWeapon[itemtype] != INVALID_WEAPON_TYPE;
}

stock AddAmmoToWeapon(itemid, amount) {
	new ItemType:ammoitem;
	GetItemWeaponItemAmmoItem(itemid, ammoitem);

	if(!IsValidItemType(ammoitem)) {
		return amount;
	}

	new
		ItemType:itemtype,
		magsize,
		reserveammo,
		maxammo,
		remainder = amount;

	itemtype = GetItemType(itemid);
	maxammo = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_maxReserveMags] * magsize;

	GetItemTypeWeaponMagSize(itemtype, magsize);
	GetItemWeaponItemReserve(itemid, reserveammo);

	dbg("weapons", "AddAmmoToWeapon",
		_i("ammoitem", _:ammoitem),
		_i("magsize", magsize));

	if(maxammo == 0) {
		// the maximum amount of ammo the weapon can hold is nothing
		dbg("weapons", "maxammo == 0",
			_i("amount", amount),
			_i("itemid", itemid),
			_i("ammoitem", _:ammoitem),
			_i("magsize", magsize),
			_i("reserveammo", reserveammo));

		if(amount > magsize) {
			remainder = (reserveammo + amount) - magsize;
			amount = magsize;
		} else {
			remainder = 0;
		}

		dbg("weapons", "updating reserve",
			_i("amount", amount));

		SetItemWeaponItemReserve(itemid, amount);
	} else {
		// the maximum amount of ammo the weapon can hold is greater than zero
		if(reserveammo == maxammo) {
			return remainder;
		}

		dbg("weapons", "adding ammo to item",
			_i("amount", amount),
			_i("itemid", itemid),
			_i("ammoitem", _:ammoitem),
			_i("reserveammo", reserveammo),
			_i("maxammo", maxammo));

		if(reserveammo + amount > maxammo) {
			remainder = (reserveammo + amount) - maxammo;
			amount = maxammo - reserveammo;
		} else {
			remainder = 0;
		}

		dbg("weapons", "updating reserve",
			_i("amount", amount));

		SetItemWeaponItemReserve(itemid, amount + reserveammo);
	}

	dbg("weapons", "finished updating weapon ammo",
		_i("remainder", remainder));

	return remainder;
}


// itmw_ItemTypeWeapon
stock GetItemTypeWeapon(ItemType:itemtype, &WeaponType:weapon) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	weapon = itmw_ItemTypeWeapon[itemtype];
	return 0;
}

// itmw_itemType
stock GetItemWeaponItemType(WeaponType:itemweaponid, &ItemType:itemtype) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	itemtype = itmw_Data[itemweaponid][itmw_itemType];
	return 0;
}

// itmw_baseWeapon
stock GetItemWeaponBaseWeapon(WeaponType:itemweaponid, &base) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	base = itmw_Data[itemweaponid][itmw_baseWeapon];
	return 0;
}

// itmw_calibre
stock GetItemWeaponCalibre(WeaponType:itemweaponid, &Calibre:calibre) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	calibre = itmw_Data[itemweaponid][itmw_calibre];
	return 0;
}

// itmw_muzzVelocity
stock GetItemWeaponMuzzVelocity(WeaponType:itemweaponid, &Float:vel) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	vel = itmw_Data[itemweaponid][itmw_muzzVelocity];
	return 0;
}

// itmw_magSize
stock GetItemWeaponMagSize(WeaponType:itemweaponid, &magsize) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	magsize = itmw_Data[itemweaponid][itmw_magSize];
	return 0;
}

// itmw_maxReserveMags
stock GetItemWeaponMaxReserveMags(WeaponType:itemweaponid, &reserve) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	reserve = itmw_Data[itemweaponid][itmw_maxReserveMags];
	return 0;
}

// itmw_animSet
stock GetItemWeaponAnimSet(WeaponType:itemweaponid, &animset) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	animset = itmw_Data[itemweaponid][itmw_animSet];
	return 0;
}

// itmw_flags
stock GetItemWeaponFlags(WeaponType:itemweaponid, &flags) {
	if(!(WeaponType:0 <= itemweaponid < WeaponType:itmw_Total)) {
		return 1;
	}

	flags = itmw_Data[itemweaponid][itmw_flags];
	return 0;
}


// -
// ItemType interface
// -


// itmw_baseWeapon
stock GetItemTypeWeaponBaseWeapon(ItemType:itemtype, &base) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	if(!(WeaponType:0 <= itmw_ItemTypeWeapon[itemtype] < WeaponType:itmw_Total)) {
		return 2;
	}

	base = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_baseWeapon];
	return 0;
}

// itmw_calibre
stock GetItemTypeWeaponCalibre(ItemType:itemtype, &Calibre:calibre) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	if(!(WeaponType:0 <= itmw_ItemTypeWeapon[itemtype] < WeaponType:itmw_Total)) {
		return 2;
	}

	calibre = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_calibre];
	return 0;
}

// itmw_muzzVelocity
stock GetItemTypeWeaponMuzzVelocity(ItemType:itemtype, &Float:vel) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	if(!(WeaponType:0 <= itmw_ItemTypeWeapon[itemtype] < WeaponType:itmw_Total)) {
		return 2;
	}

	vel = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_muzzVelocity];
	return 0;
}

// itmw_magSize
stock GetItemTypeWeaponMagSize(ItemType:itemtype, &magsize) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	if(!(WeaponType:0 <= itmw_ItemTypeWeapon[itemtype] < WeaponType:itmw_Total)) {
		return 2;
	}

	magsize = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_magSize];
	return 0;
}

// itmw_maxReserveMags
stock GetItemTypeWeaponMaxReserveMags(ItemType:itemtype, &reserve) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	if(!(WeaponType:0 <= itmw_ItemTypeWeapon[itemtype] < WeaponType:itmw_Total)) {
		return 2;
	}

	reserve = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_maxReserveMags];
	return 0;
}

// itmw_animSet
stock GetItemTypeWeaponAnimSet(ItemType:itemtype, &animset) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	if(!(WeaponType:0 <= itmw_ItemTypeWeapon[itemtype] < WeaponType:itmw_Total)) {
		return 2;
	}

	animset = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_animSet];
	return 0;
}

// itmw_flags
stock GetItemTypeWeaponFlags(ItemType:itemtype, &flags) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	if(!(WeaponType:0 <= itmw_ItemTypeWeapon[itemtype] < WeaponType:itmw_Total)) {
		return 2;
	}

	flags = itmw_Data[itmw_ItemTypeWeapon[itemtype]][itmw_flags];
	return 0;
}


// -
// Item array data interface
// -


// WEAPON_ITEM_ARRAY_CELL_MAG
stock GetItemWeaponItemMagAmmo(itemid, &ammo) {
	return GetItemArrayDataAtCell(itemid, WEAPON_ITEM_ARRAY_CELL_MAG, ammo);
}

stock SetItemWeaponItemMagAmmo(itemid, amount) {
	if(amount == 0) {
		new reserve;
		GetItemWeaponItemReserve(itemid, reserve);
		if(reserve == 0) {
			SetItemWeaponItemAmmoItem(itemid, INVALID_ITEM_TYPE);
		}
	}
	SetItemArrayDataSize(itemid, 4);
	return SetItemArrayDataAtCell(itemid, amount, WEAPON_ITEM_ARRAY_CELL_MAG);
}

// WEAPON_ITEM_ARRAY_CELL_RESERVE
stock GetItemWeaponItemReserve(itemid, &reserve) {
	return GetItemArrayDataAtCell(itemid, WEAPON_ITEM_ARRAY_CELL_RESERVE, reserve);
}

stock SetItemWeaponItemReserve(itemid, amount) {
	if(amount == 0) {
		new mag;
		GetItemWeaponItemMagAmmo(itemid, mag);
		if(mag == 0) {
			SetItemWeaponItemAmmoItem(itemid, INVALID_ITEM_TYPE);
		}
	}
	SetItemArrayDataSize(itemid, 4);
	return SetItemArrayDataAtCell(itemid, amount, WEAPON_ITEM_ARRAY_CELL_RESERVE);
}

// WEAPON_ITEM_ARRAY_CELL_AMMOITEM
stock GetItemWeaponItemAmmoItem(itemid, ItemType:itemtype) {
	return GetItemArrayDataAtCell(itemid, WEAPON_ITEM_ARRAY_CELL_AMMOITEM, _:itemtype);
}

stock SetItemWeaponItemAmmoItem(itemid, ItemType:itemtype) {
	SetItemArrayDataSize(itemid, 4);
	return SetItemArrayDataAtCell(itemid, _:itemtype, WEAPON_ITEM_ARRAY_CELL_AMMOITEM);
}

// From player

stock GetPlayerMagAmmo(playerid, &mag) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}
	return GetItemWeaponItemMagAmmo(GetPlayerItem(playerid), mag);
}

stock GetPlayerReserveAmmo(playerid, &reserve) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}
	return GetItemWeaponItemReserve(GetPlayerItem(playerid), reserve);
}

stock GetPlayerTotalAmmo(playerid, &ammo) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}

	new
		itemid = GetPlayerItem(playerid),
		mag,
		reserve;

	GetItemWeaponItemMagAmmo(itemid, mag);
	GetItemWeaponItemReserve(itemid, reserve);

	ammo = mag + reserve;
	return 0;
}
