static
	Container:ContainerID[MAX_PLAYERS],
	ContainerOptionID[MAX_PLAYERS] = {-1, ...},
	Item:TargetItem[MAX_PLAYERS] = {INVALID_ITEM_ID, ...},
	Item:SelectedItem[MAX_PLAYERS] = {INVALID_ITEM_ID, ...};


hook OnPlayerViewCntOpt(playerid, Container:containerid) {
	new
		slot,
		Item:itemid,
		ItemType:itemtype,
		WeaponType:weaponType;

	GetPlayerContainerSlot(playerid, slot);
	GetContainerSlotItem(containerid, slot, itemid);
	itemtype = GetItemType(itemid);

	if(GetItemTypeWeaponType(itemtype, weaponType)) {
		return Y_HOOKS_CONTINUE_RETURN_0;
	}

	new Calibre:calibre;
	if(GetWeaponTypeCalibre(weaponType, calibre)) {
		return Y_HOOKS_CONTINUE_RETURN_0;
	}

	if(IsValidItem(SelectedItem[playerid]) && SelectedItem[playerid] != itemid) {
		ContainerOptionID[playerid] = AddContainerOption(playerid, "Transfer Ammo Here");
		TargetItem[playerid] = itemid;
	} else {
		ContainerOptionID[playerid] = AddContainerOption(playerid, "Transfer Ammo...");
		SelectedItem[playerid] = itemid;
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerSelectCntOpt(playerid, Container:containerid, option) {
	if(option == ContainerOptionID[playerid]) {
		if(!IsValidItem(SelectedItem[playerid])) {
			return Y_HOOKS_CONTINUE_RETURN_0;
		}

		ContainerID[playerid] = containerid;

		if(SelectedItem[playerid] != TargetItem[playerid]) {
			new ret = DisplayTransferAmmoDialog(playerid);
			if(ret) {
				err("attempted to display ammo transfer dialog failed",
					_i("ret", ret));
			}
		} else {
			SelectedItem[playerid] = TargetItem[playerid];
			DisplayContainerInventory(playerid, containerid);
		}
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

DisplayTransferAmmoDialog(playerid, msg[] = "") {
	if(!IsValidItem(TargetItem[playerid])) {
		return 1;
	}

	if(!IsValidItem(SelectedItem[playerid])) {
		return 2;
	}

	new
		slot,
		Item:sourceItem,
		ItemType:sourceItemType,
		sourceItemName[MAX_ITEM_NAME],
		ItemType:targetItemType,
		targetItemName[MAX_ITEM_NAME];

	sourceItem = SelectedItem[playerid];
	sourceItemType = GetItemType(sourceItem);
	GetItemTypeName(sourceItemType, sourceItemName);
	GetPlayerContainerSlot(playerid, slot);
	targetItemType = GetItemType(TargetItem[playerid]);
	GetItemTypeName(targetItemType, targetItemName);

	new body[256];
	format(body, sizeof body, "Enter amount of ammo to transfer from %s to %s\n\n%s", sourceItemName, targetItemName, msg);
	Dialog_Show(playerid, TransferAmmo, DIALOG_STYLE_INPUT, "Transfer Ammo", body, "Accept", "Cancel");

	return 0;
}
Dialog:TransferAmmo(playerid, response, listitem, inputtext[]) {
	new
		slot,
		Item:sourceItem,
		ItemType:sourceItemType,
		sourceItemName[MAX_ITEM_NAME],
		Item:targetItem,
		ItemType:targetItemType,
		targetItemName[MAX_ITEM_NAME];

	sourceItem = SelectedItem[playerid];
	sourceItemType = GetItemType(sourceItem);
	GetItemTypeName(sourceItemType, sourceItemName);
	GetPlayerContainerSlot(playerid, slot);
	GetContainerSlotItem(ContainerID[playerid], slot, targetItem);
	targetItemType = GetItemType(targetItem);
	GetItemTypeName(targetItemType, targetItemName);

	if(response) {
		new amount = strval(inputtext);

		new WeaponType:sourceWeaponType;
		if(!GetItemTypeWeaponType(sourceItemType, sourceWeaponType)) {
			new WeaponType:targetWeaponType;
			if(!GetItemTypeWeaponType(targetItemType, targetWeaponType)) {
				// weapon to weapon
				new
					sourceitemammo,
					targetitemammo,
					AmmoType:sourceAmmoType;
				GetWeaponReserve(sourceItem, sourceitemammo);
				GetWeaponReserve(targetItem, targetitemammo);

				if(!GetWeaponAmmoType(sourceItem, sourceAmmoType) && 0 < amount <= sourceitemammo) {
					SetWeaponReserve(sourceItem, sourceitemammo - amount);
					SetWeaponReserve(targetItem, targetitemammo + amount);
					SetWeaponAmmoType(targetItem, sourceAmmoType);
				} else {
					DisplayTransferAmmoDialog(playerid, sprintf("%s only contains %d ammo", sourceItemName, sourceitemammo));
				}

				return;
			}

			new AmmoType:targetAmmoType;
			if(!GetItemTypeAmmoType(targetItemType, targetAmmoType)) {
				// weapon to ammo
				new
					sourceitemammo,
					targetitemammo;

				GetWeaponReserve(sourceItem, sourceitemammo);
				GetItemArrayDataAtCell(targetItem, targetitemammo, 0);

				if(0 < amount <= sourceitemammo) {
					SetWeaponReserve(sourceItem, sourceitemammo - amount);
					SetItemArrayDataAtCell(targetItem, targetitemammo + amount, 0);
				} else {
					DisplayTransferAmmoDialog(playerid, sprintf("%s only contains %d ammo", sourceItemName, sourceitemammo));
				}

				return;
			}
		}

		new AmmoType:sourceAmmoType;
		if(!GetItemTypeAmmoType(sourceItemType, sourceAmmoType)) {
			new WeaponType:targetWeaponType;
			if(!GetItemTypeWeaponType(targetItemType, targetWeaponType)) {
				// ammo to weapon
				new
					sourceitemammo,
					targetitemammo;

				GetItemArrayDataAtCell(sourceItem, sourceitemammo, 0);
				GetWeaponReserve(targetItem, targetitemammo);

				if(!GetWeaponAmmoType(sourceItem, sourceAmmoType) && 0 < amount <= sourceitemammo) {
					SetItemArrayDataAtCell(sourceItem, sourceitemammo - amount, 0);
					SetWeaponReserve(targetItem, targetitemammo + amount);
					SetWeaponAmmoType(targetItem, sourceAmmoType);
				} else {
					DisplayTransferAmmoDialog(playerid, sprintf("%s only contains %d ammo", sourceItemName, sourceitemammo));
				}

				return;
			}

			new AmmoType:targetAmmoType;
			if(!GetItemTypeAmmoType(targetItemType, targetAmmoType)) {
				// ammo to ammo
				new
					sourceitemammo,
					targetitemammo;

				GetItemArrayDataAtCell(sourceItem, sourceitemammo, 0);
				GetItemArrayDataAtCell(targetItem, targetitemammo, 0);

				if(0 < amount <= sourceitemammo) {
					SetItemArrayDataAtCell(sourceItem, sourceitemammo - amount, 0);
					SetItemArrayDataAtCell(targetItem, targetitemammo + amount, 0);
				} else {
					DisplayTransferAmmoDialog(playerid, sprintf("%s only contains %d ammo", sourceItemName, sourceitemammo));
				}

				return;
			}
		}
	}

	ContainerOptionID[playerid] = -1;
	SelectedItem[playerid] = INVALID_ITEM_ID;
	DisplayContainerInventory(playerid, ContainerID[playerid]);

	return;
}

hook OnPlayerUseItemWithItem(playerid, Item:itemid, Item:withitemid) {
	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_CUFFED || IsPlayerKnockedOut(playerid) || GetPlayerAnimationIndex(playerid) == 1381) {
		return Y_HOOKS_CONTINUE_RETURN_1;
	}

	_weapons_useWithItem(playerid, itemid, withitemid);

	return Y_HOOKS_CONTINUE_RETURN_0;
}

_weapons_useWithItem(playerid, Item:heldItemID, Item:targetItemID) {
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

_weapons_transferWeaponToWeapon(playerid, Item:heldItemID, Item:targetItemID, WeaponType:heldWeaponType, WeaponType:targetWeaponType) {
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
	defer _weapons_timerWeapToWeap(playerid, _:targetItemID, _:heldItemID);

	return 0;
}
timer _weapons_timerWeapToWeap[400](playerid, srcItemID, trgItemID) {
	new
		magammo,
		reserveammo;

	GetWeaponMagAmmo(Item:srcItemID, magammo);
	GetWeaponReserve(Item:srcItemID, reserveammo);

	if(reserveammo + magammo > 0) {
		new
			AmmoType:ammoType,
			remainder;

		GetWeaponAmmoType(Item:srcItemID, ammoType);
		SetWeaponAmmoType(Item:trgItemID, ammoType);
		GivePlayerAmmo(playerid, reserveammo + magammo, remainder);

		SetWeaponMagAmmo(Item:srcItemID, 0);
		SetWeaponReserve(Item:srcItemID, remainder);

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

_weapons_transferAmmoToWeapon(playerid, Item:heldItemID, Item:targetItemID, WeaponType:heldWeaponType, AmmoType:targetAmmoType) {
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
	// 	SetWeaponMagAmmo(heldItemID, GetWeaponMagAmmo(heldItemID) + floatround(transfer) * 100);
	// 	SetWeaponAmmoItem(heldItemID, item_GasCan);
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
	defer _weapons_timerAmmoToWeapon(playerid, _:targetItemID, _:heldItemID);

	return;
}
timer _weapons_timerAmmoToWeapon[400](playerid, srcItemID, trgItemID) {
	new ammo;

	if(IsError(GetItemArrayDataAtCell(Item:srcItemID, ammo, 0))) {
		Handled();
		return;
	}

	if(ammo > 0) {
		new
			AmmoType:ammoType,
			remainder;

		GetItemTypeAmmoType(GetItemType(Item:srcItemID), ammoType);

		GivePlayerAmmo(playerid, ammo, remainder);
		SetWeaponAmmoType(Item:trgItemID, ammoType);
		SetItemArrayDataAtCell(Item:srcItemID, remainder, 0);

		dbg("weapon", "transfered from ammo item to weapon item",
			_i("ammo", ammo),
			_i("ammoType", _:ammoType),
			_i("remainder", remainder));

		new message[128];
		format(message, sizeof message, @L(playerid, "AMTRANSTTOW", true), ammo - remainder);
		ShowActionText(playerid, message, 3000);
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, 0, 0, 0, 0, 0);
	return;
}


// -
// Transfer ammo from world weapon to held ammo
// -

_weapons_transferWeaponToAmmo(playerid, Item:heldItemID, Item:targetItemID, AmmoType:heldAmmoType, WeaponType:targetWeaponType) {
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
	GetWeaponAmmoType(Item:targetItemID, targetAmmoType);

	if(IsValidAmmoType(heldAmmoType)) {
		if(targetAmmoType != heldAmmoType) {
			ShowActionText(playerid, @L(playerid, "AMDIFFATYPE", true), 5000);
			return;
		}
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, 1, 0, 0, 0, 450);
	defer _weapons_timerWeaponToAmmo(playerid, _:targetItemID, _:heldItemID);

	return;
}
timer _weapons_timerWeaponToAmmo[400](playerid, srcItemID, trgItemID) {
	new
		existing,
		mag,
		reserve,
		total;

	GetItemArrayDataAtCell(Item:trgItemID, existing, 0);
	GetWeaponMagAmmo(Item:srcItemID, mag);
	GetWeaponReserve(Item:srcItemID, reserve);

	total = (existing + mag + reserve);

	SetItemArrayDataAtCell(Item:trgItemID, total, 0);

	SetWeaponMagAmmo(Item:srcItemID, 0);
	SetWeaponReserve(Item:srcItemID, 0);

	new message[128];
	format(message, sizeof message, @L(playerid, "AMTRANSWTOT", true), mag + reserve);
	ShowActionText(playerid, message, 3000);

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, 0, 0, 0, 0, 0);
}


// -
// Transfer ammo from world ammo to held ammo
// -

_weapons_transferAmmoToAmmo(playerid, Item:heldItemID, Item:targetItemID, AmmoType:heldAmmoType, AmmoType:targetAmmoType) {
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
	defer _weapons_timerAmmoToAmmo(playerid, _:targetItemID, _:heldItemID);

	return;
}
timer _weapons_timerAmmoToAmmo[400](playerid, srcItemID, trgItemID) {
	new
		existing,
		amount;

	GetItemArrayDataAtCell(Item:trgItemID, existing, 0),
	GetItemArrayDataAtCell(Item:srcItemID, amount, 0);

	SetItemArrayDataAtCell(Item:trgItemID, existing + amount, 0);
	SetItemArrayDataAtCell(Item:srcItemID, 0, 0);

	new message[128];
	format(message, sizeof message, @L(playerid, "AMTRANSTTOT", true), amount);
	ShowActionText(playerid, message, 3000);

	ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, 0, 0, 0, 0, 0);
}
