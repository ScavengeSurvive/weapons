// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_damage_generic
	#undef _inc_damage_generic
#endif
// custom include-guard to ensure we don't duplicate
#if defined _damage_generic_included
	#endinput
#endif
#define _damage_generic_included


#include <YSI\y_hooks>


hook OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart) {
	switch(weaponid) {
		case 31: {
			new model = GetVehicleModel(GetPlayerVehicleID(playerid));

			if(model == 447 || model == 476) {
				_firearm_doDamage(issuerid, playerid, INVALID_ITEM_ID, WeaponType:0, bodypart);
			}
		}
		case 38: {
			if(GetVehicleModel(GetPlayerVehicleID(playerid)) == 425) {
				_firearm_doDamage(issuerid, playerid, INVALID_ITEM_ID, WeaponType:0, bodypart);
			}
		}
	}

	return 1;
}
