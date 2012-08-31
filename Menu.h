
#if defined(CONDITION_MENU) || defined(ACTION_MENU) || defined(EXPRESSION_MENU)
	EditData ed;
	ed.Deserialize(edPtr);
	if(::LastMenu == 0)
	{
		LastMenu = ed.LastMenu;
	}
#endif

// --------------------------------
// Condition menu
// --------------------------------

#ifdef CONDITION_MENU

	SEPARATOR
	if(::LastMenu != 1)
	{SUB_START("Contiguous Lists")}
	else
	{DISABLED(2, "Contiguous Lists:")}
		ITEM(0, "Item Exists in List")
		ITEM(3, "Item matching Regex Exists in List")
	if(::LastMenu != 1)
	{SUB_END}
	if(::LastMenu != 0)
	{SEPARATOR}
	if(::LastMenu != 2)
	{SUB_START("Linked Lists")}
	else
	{DISABLED(2,"Linked Lists:")}
		ITEM(4, "Iterator is at beginning")
		ITEM(5, "Iterator is at end")
	if(::LastMenu != 2)
	{SUB_END}
	SEPARATOR
	ITEM(1, "On Custom Sort")
	SEPARATOR
	ITEM(2, "On Error")

#endif

// --------------------------------
// Action menu
// --------------------------------

#ifdef ACTION_MENU

	SEPARATOR
	if(::LastMenu != 1)
	{SUB_START("Contiguous Lists")}
	else
	{DISABLED(4, "Contiguous Lists:")}
		ITEM(0, "Insert Item")
		ITEM(1, "Change Item")
		SEPARATOR
		ITEM(2, "Copy Items in Range")
		ITEM(14,"Copy Items from another Contiguous List")
		ITEM(15,"Copy Items from a Linked List")
		SEPARATOR
		ITEM(3, "Delete Items in Range")
		ITEM(11,"Delete Items matching Regex")
		ITEM(4, "Clear List")
		ITEM(10,"Reserve More Space")
		SEPARATOR
		ITEM(5, "Save List to File")
		ITEM(6, "Load List from File")
		ITEM(7, "Load List from String")
		SEPARATOR
		ITEM(8, "Sort List in Range")
		ITEM(9, "Custom Sort: Return True/False")
		SEPARATOR
		ITEM(12,"Change Editing List")
		ITEM(13,"Delete List")
	if(::LastMenu != 1)
	{SUB_END}
	if(::LastMenu != 0)
	{SEPARATOR}
	if(::LastMenu != 2)
	{SUB_START("Linked Lists")}
	else
	{DISABLED(4, "Linked Lists:")}
		ITEM(16,"Iterate Forward")
		ITEM(17,"Iterate Backward")
		ITEM(18,"Iterate to Beginning")
		ITEM(19,"Iterate to End")
		SEPARATOR
		ITEM(20,"Insert Item")
		ITEM(21,"Change Item")
		SEPARATOR
		ITEM(22,"Copy Items in Range")
		ITEM(23,"Copy Items from another Linked List")
		ITEM(24,"Copy Items from a Contiguous List")
		SEPARATOR
		ITEM(25,"Delete Items in Range")
		ITEM(26,"Clear List")
		SEPARATOR
		ITEM(27,"Save List to File")
		ITEM(28,"Load List from File")
		ITEM(29,"Load List from String")
		SEPARATOR
		ITEM(30,"Sort List in Range")
		ITEM(9,"Custom Sort: Return True/False")
		SEPARATOR
		ITEM(31,"Change Editing List")
		ITEM(32,"Change Editing Iterator")
		ITEM(33,"Delete List")
		ITEM(34,"Delete Iterator")
	if(::LastMenu != 2)
	{SUB_END}
	SEPARATOR

#endif


// --------------------------------
// Expression menu
// --------------------------------

#ifdef EXPRESSION_MENU

	SEPARATOR
	if(::LastMenu != 1)
	{SUB_START("Contiguous Lists")}
	else
	{DISABLED(1, "Contiguous Lists:")}
		ITEM(0, "Get Item")
		ITEM(1, "Get Number of Items")
		SEPARATOR
		ITEM(2, "Get Number of Item exact matches")
		ITEM(3, "Get Index of Item exact match by Occurrence")
		ITEM(9, "Get Number of Items matching Regex")
		ITEM(10,"Get Index of Item matching Regex by Occurrence")
		SEPARATOR
		ITEM(4, "Get List as String")
		SEPARATOR
		ITEM(5, "Custom Sort: Get String A")
		ITEM(6, "Custom Sort: Get String B")
		SEPARATOR
		ITEM(7, "Get Capacity")
		SEPARATOR
		ITEM(11,"Get Current Editing List")
		ITEM(12,"Get Number of Lists")
		ITEM(13,"Get List Name by Index")
		ITEM(14,"Change List (String Expressions)")
		ITEM(15,"Change List (Number Expressions)")
	if(::LastMenu != 1)
	{SUB_END}
	if(::LastMenu != 0)
	{SEPARATOR}
	if(::LastMenu != 2)
	{SUB_START("Linked Lists")}
	else
	{DISABLED(17,"Linked Lists:")}
		ITEM(16,"Get Item")
		ITEM(17,"Get Number of Items")
		SEPARATOR
		ITEM(18,"Get List as String")
		SEPARATOR
		ITEM(5, "Custom Sort: Get String A")
		ITEM(6, "Custom Sort: Get String B")
		SEPARATOR
		ITEM(19,"Get Current Editing List")
		ITEM(20,"Get Current Editing Iterator")
		ITEM(21,"Get Number of Lists")
		ITEM(22,"Get List Name by Index")
		ITEM(23,"Change List (String Expressions)")
		ITEM(24,"Change List (Number Expressions)")
	if(::LastMenu != 2)
	{SUB_END}
	SEPARATOR

#endif