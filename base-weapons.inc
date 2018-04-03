// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_base_weapons
	#undef _inc_base_weapons
#endif
// custom include-guard to ensure we don't duplicate
#if defined _base_weapons_included
	#endinput
#endif
#define _base_weapons_included


// base-weapons contains basic utility functions for working with GTA weapon IDs.


stock IsBaseWeaponMelee(weaponid) {
	switch(weaponid) {
		case 1..15:
			return 1;
	}
	return 0;
}

stock IsBaseWeaponThrowable(weaponid) {
	switch(weaponid) {
		case 16..18, 39:
			return 1;
	}
	return 0;
}

stock IsBaseWeaponClipBased(weaponid) {
	switch(weaponid) {
		case 22..38, 43:
			return 1;
	}
	return 0;
}

stock IsBaseWeaponNoAmmo(weaponid) {
	switch(weaponid) {
		case 1..18, 39, 40, 44..45:
			return 1;
	}
	return 0;
}

stock IsBaseWeaponDriveby(weaponid) {
	switch(weaponid) {
		case 28, 29, 32: {
			return 1;
		}
	}
	return 0;
}

stock IsWeaponMelee(itemid) {
	return IsBaseWeaponMelee(GetItemWeaponBaseWeapon(GetItemTypeWeapon(GetItemType(itemid))));
}

stock IsWeaponThrowable(itemid) {
	return IsBaseWeaponThrowable(GetItemWeaponBaseWeapon(GetItemTypeWeapon(GetItemType(itemid))));
}

stock IsWeaponClipBased(itemid) {
	return IsBaseWeaponClipBased(GetItemWeaponBaseWeapon(GetItemTypeWeapon(GetItemType(itemid))));
}

stock IsWeaponNoAmmo(itemid) {
	return IsBaseWeaponNoAmmo(GetItemWeaponBaseWeapon(GetItemTypeWeapon(GetItemType(itemid))));
}

stock IsWeaponDriveby(itemid) {
	return IsBaseWeaponDriveby(GetItemWeaponBaseWeapon(GetItemTypeWeapon(GetItemType(itemid))));
}
