static
	trans_ContainerID[MAX_PLAYERS],
	trans_ContainerOptionID[MAX_PLAYERS] = {-1, ...},
	trans_SelectedItem[MAX_PLAYERS] = {INVALID_ITEM_ID, ...};


hook OnPlayerViewCntOpt(playerid, containerid)
{
	new
		itemid,
		ItemType:itemtype;

	itemid = GetContainerSlotItem(containerid, GetPlayerContainerSlot(playerid));
	itemtype = GetItemType(itemid);

	if((GetItemTypeWeapon(itemtype) != -1 && GetItemTypeWeaponCalibre(itemtype) != -1) || GetItemTypeAmmoType(itemtype) != -1)
	{
		if(IsValidItem(trans_SelectedItem[playerid]) && trans_SelectedItem[playerid] != itemid)
		{
			trans_ContainerOptionID[playerid] = AddContainerOption(playerid, "Transfer Ammo Here");
		}
		else
		{
			trans_ContainerOptionID[playerid] = AddContainerOption(playerid, "Transfer Ammo...");
		}
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerSelectCntOpt(playerid, containerid, option)
{
	if(option == trans_ContainerOptionID[playerid])
	{
		trans_ContainerID[playerid] = playerid;
		if(IsValidItem(trans_SelectedItem[playerid]) && trans_SelectedItem[playerid] != GetContainerSlotItem(containerid, GetPlayerContainerSlot(playerid)))
		{
			DisplayTransferAmmoDialog(playerid, containerid);
		}
		else
		{
			trans_SelectedItem[playerid] = GetContainerSlotItem(containerid, GetPlayerContainerSlot(playerid));
			DisplayContainerInventory(playerid, containerid);
		}
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}

DisplayTransferAmmoDialog(playerid, containerid, msg[] = "")
{
	new
		sourceitemid,
		ItemType:sourceitemtype,
		sourceitemname[ITM_MAX_NAME],
		targetitemid,
		ItemType:targetitemtype,
		targetitemname[ITM_MAX_NAME];

	sourceitemid = trans_SelectedItem[playerid];
	sourceitemtype = GetItemType(sourceitemid);
	GetItemTypeName(sourceitemtype, sourceitemname);
	targetitemid = GetContainerSlotItem(containerid, GetPlayerContainerSlot(playerid));
	targetitemtype = GetItemType(targetitemid);
	GetItemTypeName(targetitemtype, targetitemname);

	Dialog_Show(playerid, TransferAmmo, DIALOG_STYLE_INPUT, "Transfer Ammo", sprintf("Enter amount of ammo to transfer from %s to %s\n\n%s", sourceitemname, targetitemname, msg), "Accept", "Cancel");
}

Dialog:TransferAmmo(playerid, response, listitem, inputtext[])
{
	new
		sourceitemid,
		ItemType:sourceitemtype,
		sourceitemname[ITM_MAX_NAME],
		targetitemid,
		ItemType:targetitemtype,
		targetitemname[ITM_MAX_NAME];

	sourceitemid = trans_SelectedItem[playerid];
	sourceitemtype = GetItemType(sourceitemid);
	GetItemTypeName(sourceitemtype, sourceitemname);
	targetitemid = GetContainerSlotItem(trans_ContainerID[playerid], GetPlayerContainerSlot(playerid));
	targetitemtype = GetItemType(targetitemid);
	GetItemTypeName(targetitemtype, targetitemname);

	if(response)
	{
		new amount = strval(inputtext);

		if(GetItemTypeWeapon(sourceitemtype) != -1)
		{
			if(GetItemTypeWeapon(targetitemtype) != -1)
			{
				// weapon to weapon
				new
					sourceitemammo = GetItemWeaponItemReserve(sourceitemid),
					targetitemammo = GetItemWeaponItemReserve(targetitemid);

				if(0 < amount <= sourceitemammo)
				{
					SetItemWeaponItemReserve(sourceitemid, sourceitemammo - amount);
					SetItemWeaponItemReserve(targetitemid, targetitemammo + amount);
					SetItemWeaponItemAmmoItem(targetitemid, sourceitemtype);
				}
				else
				{
					DisplayTransferAmmoDialog(playerid, trans_ContainerID[playerid], sprintf("%s only contains %d ammo", sourceitemname, sourceitemammo));
				}

			}
			else if(GetItemTypeAmmoType(targetitemtype) != -1)
			{
				// weapon to ammo
				new
					sourceitemammo = GetItemWeaponItemReserve(sourceitemid),
					targetitemammo = GetItemArrayDataAtCell(targetitemid, 0);

				if(0 < amount <= sourceitemammo)
				{
					SetItemWeaponItemReserve(sourceitemid, sourceitemammo - amount);
					SetItemArrayDataAtCell(targetitemid, targetitemammo + amount, 0);
				}
				else
				{
					DisplayTransferAmmoDialog(playerid, trans_ContainerID[playerid], sprintf("%s only contains %d ammo", sourceitemname, sourceitemammo));
				}
			}
		}
		else if(GetItemTypeAmmoType(sourceitemtype) != -1)
		{
			if(GetItemTypeWeapon(targetitemtype) != -1)
			{
				// ammo to weapon
				new
					sourceitemammo = GetItemArrayDataAtCell(sourceitemid, 0),
					targetitemammo = GetItemWeaponItemReserve(targetitemid);

				if(0 < amount <= sourceitemammo)
				{
					SetItemArrayDataAtCell(sourceitemid, sourceitemammo - amount, 0);
					SetItemWeaponItemReserve(targetitemid, targetitemammo + amount);
					SetItemWeaponItemAmmoItem(targetitemid, sourceitemtype);
				}
				else
				{
					DisplayTransferAmmoDialog(playerid, trans_ContainerID[playerid], sprintf("%s only contains %d ammo", sourceitemname, sourceitemammo));
				}
			}
			else if(GetItemTypeAmmoType(targetitemtype) != -1)
			{
				// ammo to ammo
				new
					sourceitemammo = GetItemArrayDataAtCell(sourceitemid, 0),
					targetitemammo = GetItemArrayDataAtCell(targetitemid, 0);

				if(0 < amount <= sourceitemammo)
				{
					SetItemArrayDataAtCell(sourceitemid, sourceitemammo - amount, 0);
					SetItemArrayDataAtCell(targetitemid, targetitemammo + amount, 0);
				}
				else
				{
					DisplayTransferAmmoDialog(playerid, trans_ContainerID[playerid], sprintf("%s only contains %d ammo", sourceitemname, sourceitemammo));
				}
			}
		}
	}

	trans_ContainerOptionID[playerid] = -1;
	trans_SelectedItem[playerid] = INVALID_ITEM_ID;
	DisplayContainerInventory(playerid, trans_ContainerID[playerid]);
}
