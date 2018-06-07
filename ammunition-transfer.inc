hook OnPlayerUseItemWithItem(playerid, itemid, withitemid) {
	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_CUFFED || IsPlayerKnockedOut(playerid) || GetPlayerAnimationIndex(playerid) == 1381) {
		return Y_HOOKS_CONTINUE_RETURN_1;
	}

	_weapons_useWithItem(playerid, itemid, withitemid);

	return Y_HOOKS_CONTINUE_RETURN_0;
}

_weapons_useWithItem(playerid, heldItemID, targetItemID) {
	new
		ItemType:heldItemType = GetItemType(heldItemID),
		ItemType:targetItemType = GetItemType(targetItemID);

	new WeaponType:heldWeaponType;
	if(!GetItemTypeWeaponType(heldItemType, heldWeaponType)) {
		// Player is holding a weapon

		new WeaponType:targetWeaponType;
		if(!GetItemTypeWeaponType(targetItemType, targetWeaponType)) {
			dbg("weapon", "player holding weapon interacted with weapon",
				_i("playerid", playerid),
				_i("heldItemType", _:heldItemType),
				_i("heldWeaponType", _:heldWeaponType),
				_i("targetItemType", _:targetItemType),
				_i("targetWeaponType", _:targetWeaponType)
			);
			_weapons_transferWeaponToWeapon(playerid, heldItemID, targetItemID, heldWeaponType, targetWeaponType);
			return;
		}

		new AmmoType:targetAmmoType;
		if(!GetItemTypeAmmoType(targetItemType, targetAmmoType)) {
			dbg("weapon", "player holding weapon interacted with ammo",
				_i("playerid", playerid),
				_i("heldItemType", _:heldItemType),
				_i("heldWeaponType", _:heldWeaponType),
				_i("targetItemType", _:targetItemType),
				_i("targetAmmoType", _:targetAmmoType)
			);
			_weapons_transferAmmoToWeapon(playerid, heldItemID, targetItemID, heldWeaponType, targetAmmoType);
			return;
		}
	}

	new AmmoType:heldAmmoType;
	if(!GetItemTypeAmmoType(heldItemType, heldAmmoType)) {
		// Player is holding an ammo item

		new WeaponType:targetWeaponType;
		if(!GetItemTypeWeaponType(targetItemType, targetWeaponType)) {
			dbg("weapon", "player holding ammo interacted with weapon",
				_i("playerid", playerid),
				_i("heldItemType", _:heldItemType),
				_i("heldAmmoType", _:heldAmmoType),
				_i("targetItemType", _:targetItemType),
				_i("targetWeaponType", _:targetWeaponType)
			);
			_weapons_transferWeaponToAmmo(playerid, heldItemID, targetItemID, heldAmmoType, targetWeaponType);
			return;
		}
		
		new AmmoType:targetAmmoType;
		if(!GetItemTypeAmmoType(targetItemType, targetAmmoType)) {
			dbg("weapon", "player holding ammo interacted with ammo",
				_i("playerid", playerid),
				_i("heldItemType", _:heldItemType),
				_i("heldAmmoType", _:heldAmmoType),
				_i("targetItemType", _:targetItemType),
				_i("targetAmmoType", _:targetAmmoType)
			);
			_weapons_transferAmmoToAmmo(playerid, heldItemID, targetItemID, heldAmmoType, targetAmmoType);
			return;
		}
	}

	return;
}


// -
// Transfer ammo from world weapon to held weapon
// -

_weapons_transferWeaponToWeapon(playerid, heldItemID, targetItemID, WeaponType:heldWeaponType, WeaponType:targetWeaponType) {
	new Calibre:heldCalibre;
	GetWeaponTypeCalibre(heldWeaponType, heldCalibre);

	if(heldCalibre == NO_CALIBRE) {
		return 1;
	}

	new flags;
	GetWeaponTypeFlags(heldWeaponType, flags);

	if(flags & WEAPON_FLAG_LIQUID_AMMO) {
		return 1;
	}

	new Calibre:targetCalibre;
	GetWeaponTypeCalibre(targetWeaponType, targetCalibre);

	if(heldCalibre != targetCalibre) {
		ShowActionText(playerid, @L(playerid, "AMWRONGCALI", true), 3000);
		return 1;
	}

	new AmmoType:heldAmmoType;
	GetWeaponAmmoType(heldItemID, heldAmmoType);

	if(IsValidAmmoType(heldAmmoType)) {
		new AmmoType:targetAmmoType;
		GetWeaponAmmoType(targetItemID, targetAmmoType);
		if(heldAmmoType != targetAmmoType) {
			ShowActionText(playerid, @L(playerid, "AMDIFFATYPE", true), 5000);
			return 1;
		}
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, 1, 0, 0, 0, 450);
	defer _weapons_timerWeapToWeap(playerid, targetItemID, heldItemID);

	return 0;
}
timer _weapons_timerWeapToWeap[400](playerid, srcItemID, trgItemID) {
	new
		magammo,
		reserveammo;

	GetWeaponMagAmmo(srcItemID, magammo);
	GetWeaponReserve(srcItemID, reserveammo);

	if(reserveammo + magammo > 0) {
		new
			AmmoType:ammoType,
			remainder;

		GetWeaponAmmoType(srcItemID, ammoType);
		SetWeaponAmmoType(trgItemID, ammoType);
		GivePlayerAmmo(playerid, reserveammo + magammo, remainder);

		SetWeaponMagAmmo(srcItemID, 0);
		SetWeaponReserve(srcItemID, remainder);

		dbg("weapon", "transfered from weapon item to weapon item",
			_i("magammo", magammo),
			_i("reserveammo", reserveammo),
			_i("ammoType", _:ammoType),
			_i("remainder", remainder));

		new message[128];
		format(message, sizeof message, @L(playerid, "AMTRANSWTOW", true), (reserveammo + magammo) - remainder);
		ShowActionText(playerid, message, 3000);
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, 0, 0, 0, 0, 0);
}


// -
// Transfer ammo from world ammo to held weapon
// -

_weapons_transferAmmoToWeapon(playerid, heldItemID, targetItemID, WeaponType:heldWeaponType, AmmoType:targetAmmoType) {
	new Calibre:heldCalibre;
	GetWeaponTypeCalibre(heldWeaponType, heldCalibre);

	if(heldCalibre == NO_CALIBRE) {
		return;
	}

	// TODO: reimplement liquid items
	// if(GetItemWeaponFlags(heldWeaponType) & WEAPON_FLAG_LIQUID_AMMO)
	// {
	// 	// heldcalibre represents a liquidtype

	// 	if(GetItemTypeLiquidContainerType(GetItemType(targetItemID)) == -1)
	// 		return;

	// 	new
	// 		Float:canfuel,
	// 		Float:transfer;

	// 	canfuel = GetLiquidItemLiquidAmount(targetItemID);

	// 	if(canfuel <= 0.0)
	// 	{
	// 		ShowActionText(playerid, ls(playerid, "EMPTY", true), 3000);
	// 		return;
	// 	}

	// 	transfer = (canfuel - 1.0 < 0.0) ? canfuel : 1.0;
	// 	SetLiquidItemLiquidAmount(targetItemID, canfuel - transfer);
	// 	SetItemWeaponItemMagAmmo(heldItemID, GetItemWeaponItemMagAmmo(heldItemID) + floatround(transfer) * 100);
	// 	SetItemWeaponItemAmmoItem(heldItemID, item_GasCan);
	// 	UpdatePlayerWeaponItem(playerid);
	// 	// todo: remove dependency on itemtypes for liquid based weaps

	// 	return;
	// }

	new Calibre:targetCalibre;
	GetAmmoTypeCalibre(targetAmmoType, targetCalibre);

	if(heldCalibre != targetCalibre) {
		ShowActionText(playerid, @L(playerid, "AMWRONGCALI", true), 3000);
		return;
	}

	new AmmoType:heldAmmoType;
	GetWeaponAmmoType(heldItemID, heldAmmoType);

	if(IsValidAmmoType(heldAmmoType)) {
		if(heldAmmoType != targetAmmoType) {
			ShowActionText(playerid, @L(playerid, "AMDIFFATYPE", true), 5000);
			return;
		}
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, 1, 0, 0, 0, 450);
	defer _weapons_timerAmmoToWeapon(playerid, targetItemID, heldItemID);

	return;
}
timer _weapons_timerAmmoToWeapon[400](playerid, srcItemID, trgItemID)
{
	new ammo;

	GetItemArrayDataAtCell(srcItemID, 0, ammo);

	if(ammo > 0) {
		new
			AmmoType:ammoType,
			remainder;

		GetItemTypeAmmoType(GetItemType(srcItemID), ammoType);

		GivePlayerAmmo(playerid, ammo, remainder);
		SetWeaponAmmoType(trgItemID, ammoType);
		SetItemArrayDataAtCell(srcItemID, remainder, 0);

		dbg("weapon", "transfered from ammo item to weapon item",
			_i("ammo", ammo),
			_i("ammoType", _:ammoType),
			_i("remainder", remainder));

		new message[128];
		format(message, sizeof message, @L(playerid, "AMTRANSTTOW", true), ammo - remainder);
		ShowActionText(playerid, message, 3000);
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, 0, 0, 0, 0, 0);
}


// -
// Transfer ammo from world weapon to held ammo
// -

_weapons_transferWeaponToAmmo(playerid, heldItemID, targetItemID, AmmoType:heldAmmoType, WeaponType:targetWeaponType) {
	new Calibre:heldCalibre;
	GetAmmoTypeCalibre(heldAmmoType, heldCalibre);

	if(heldCalibre == NO_CALIBRE) {
		return;
	}

	new flags;
	GetWeaponTypeFlags(targetWeaponType, flags);
	if(flags & WEAPON_FLAG_LIQUID_AMMO) {
		return;
	}

	new Calibre:targetCalibre;
	GetWeaponTypeCalibre(targetWeaponType, targetCalibre);
	if(heldCalibre != targetCalibre) {
		ShowActionText(playerid, @L(playerid, "AMWRONGCALI", true), 3000);
		return;
	}

	new AmmoType:targetAmmoType;
	GetWeaponAmmoType(targetItemID, targetAmmoType);

	if(IsValidAmmoType(heldAmmoType)) {
		if(targetAmmoType != heldAmmoType) {
			ShowActionText(playerid, @L(playerid, "AMDIFFATYPE", true), 5000);
			return;
		}
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, 1, 0, 0, 0, 450);
	defer _weapons_timerWeaponToAmmo(playerid, targetItemID, heldItemID);

	return;
}
timer _weapons_timerWeaponToAmmo[400](playerid, srcItemID, trgItemID) {
	new
		existing,
		mag,
		reserve,
		total;

	GetItemArrayDataAtCell(trgItemID, 0, existing);
	GetWeaponMagAmmo(srcItemID, mag);
	GetWeaponReserve(srcItemID, reserve);

	total = (existing + mag + reserve);

	SetItemArrayDataAtCell(trgItemID, total, 0);

	SetWeaponMagAmmo(srcItemID, 0);
	SetWeaponReserve(srcItemID, 0);

	new message[128];
	format(message, sizeof message, @L(playerid, "AMTRANSWTOT", true), mag + reserve);
	ShowActionText(playerid, message, 3000);

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, 0, 0, 0, 0, 0);
}


// -
// Transfer ammo from world ammo to held ammo
// -

_weapons_transferAmmoToAmmo(playerid, heldItemID, targetItemID, AmmoType:heldAmmoType, AmmoType:targetAmmoType) {
	new
		Calibre:heldCalibre,
		Calibre:targetCalibre;
	GetAmmoTypeCalibre(heldAmmoType, heldCalibre);
	GetAmmoTypeCalibre(targetAmmoType, targetCalibre);

	if(heldCalibre == NO_CALIBRE || targetCalibre == NO_CALIBRE) {
		return;
	}

	if(heldCalibre != targetCalibre) {
		ShowActionText(playerid, @L(playerid, "AMMIXINTINS", true), 5000);
		return;
	}

	if(heldAmmoType != targetAmmoType) {
		ShowActionText(playerid, @L(playerid, "AMMIXINTINS", true), 5000);
		return;
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, 1, 0, 0, 0, 450);
	defer _weapons_timerAmmoToAmmo(playerid, targetItemID, heldItemID);

	return;
}
timer _weapons_timerAmmoToAmmo[400](playerid, srcItemID, trgItemID) {
	new
		existing,
		amount;

	GetItemArrayDataAtCell(trgItemID, 0, existing),
	GetItemArrayDataAtCell(srcItemID, 0, amount);

	SetItemArrayDataAtCell(trgItemID, existing + amount, 0);
	SetItemArrayDataAtCell(srcItemID, 0, 0);

	new message[128];
	format(message, sizeof message, @L(playerid, "AMTRANSTTOT", true), amount);
	ShowActionText(playerid, message, 3000);

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, 0, 0, 0, 0, 0);
}
