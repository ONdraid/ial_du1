/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
	// Initialize inner variables
	list->activeElement = NULL;
	list->firstElement = NULL;
	list->lastElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {
	// Free all elements "backwards"
	while (list->lastElement != NULL)
	{
		DLLElementPtr toDelete = list->lastElement;
		list->lastElement = list->lastElement->previousElement;
		free(toDelete);
	}
	// And reset inner variables
	list->firstElement = NULL;
	list->activeElement = NULL;
	list->currentLength = 0;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, long data ) {
	DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
	if (newElem == NULL) {
		DLL_Error();
		return;
	}

	// Set new element data
	newElem->data = data;

	// Set new element pointers
	newElem->nextElement = list->firstElement;
	newElem->previousElement = NULL;

	if (list->firstElement != NULL)
		list->firstElement->previousElement = newElem; // If there is a first element, rewire its previous pointer
	else
		list->lastElement = newElem; // Otherwise, set new element as the last element

	// Set new element as the first element and increment the length
	list->firstElement = newElem;
	list->currentLength++;
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, long data ) {
	DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
	if (newElem == NULL) {
		DLL_Error();
		return;
	}
	
	// Set new element data
	newElem->data = data;

	// Set new element pointers
	newElem->nextElement = NULL;
	newElem->previousElement = list->lastElement;

	if (list->lastElement != NULL)
		list->lastElement->nextElement = newElem; // If there is a last element, rewire its next pointer
	else
		list->firstElement = newElem; // Otherwise, set new element as the first element

	// Set new element as the last element and increment the length
	list->lastElement = newElem;
	list->currentLength++;
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
	list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {
	list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, long *dataPtr ) {
	if (list->firstElement == NULL) {
		DLL_Error();
		return;
	}
	
	// Set data
	*dataPtr = list->firstElement->data;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, long *dataPtr ) {
	if (list->lastElement == NULL) {
		DLL_Error();
		return;
	}
	
	// Set data
	*dataPtr = list->lastElement->data;
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {
	if (list->firstElement == NULL)	return;
	if (list->firstElement == list->activeElement) list->activeElement = NULL;

	// Backup first element
	DLLElementPtr firstBackup = list->firstElement;
	// Set first element
	list->firstElement = list->firstElement->nextElement;
	
	// If list is not empty, rewire the new first element
	if (list->firstElement != NULL) list->firstElement->previousElement = NULL;
	else list->lastElement = NULL; // Otherwise, set last element to NULL

	// Free element and decrement the length
	free(firstBackup);
	list->currentLength--;
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {
	if (list->lastElement == NULL)	return;
	if (list->lastElement == list->activeElement) list->activeElement = NULL;

	// Backup last element
	DLLElementPtr lastBackup = list->lastElement;

	// Set last element
	list->lastElement = list->lastElement->previousElement;

	// If list is not empty, rewire the new last element
	if (list->lastElement != NULL) list->lastElement->nextElement = NULL;
	else list->firstElement = NULL; // Otherwise, set first element to NULL

	// Free element and decrement the length
	free(lastBackup);
	list->currentLength--;
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
	// If list is not active or empty, return
	if (list->activeElement == NULL || list->activeElement == list->lastElement) return;

	// Backup element to delete
	DLLElementPtr toDelete = list->activeElement->nextElement;

	// Rewire pointers
	list->activeElement->nextElement = toDelete->nextElement;

	// If there is a next element, rewire its previous pointer
	if (toDelete->nextElement != NULL) toDelete->nextElement->previousElement = list->activeElement;
	else list->lastElement = list->activeElement; // Otherwise, set last element to active element

	// Free element and decrement the length
	free(toDelete);
	list->currentLength--;
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
	// If list is not active or empty, return
	if (list->activeElement == NULL || list->activeElement == list->firstElement) return;

	// Backup element to delete
	DLLElementPtr toDelete = list->activeElement->previousElement;

	// Rewire pointers
	list->activeElement->previousElement = toDelete->previousElement;

	// If there is a previous element, rewire its next pointer
	if (toDelete->previousElement != NULL) toDelete->previousElement->nextElement = list->activeElement;
	else list->firstElement = list->activeElement; // Otherwise, set first element to active element

	// Free element and decrement the length
	free(toDelete);
	list->currentLength--;
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, long data ) {
	if (list->activeElement == NULL) return;

	// Allocate memory for new element
	DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
	if (newElem == NULL) {
		DLL_Error();
		return;
	}

	// Set its data
	newElem->data = data;

	// Rewire pointers
	DLLElementPtr nextBackup = list->activeElement->nextElement;
	list->activeElement->nextElement = newElem;
	newElem->previousElement = list->activeElement;
	newElem->nextElement = nextBackup;

	// If there is a next element, rewire its previous pointer
	if (nextBackup != NULL) nextBackup->previousElement = newElem;
	else list->lastElement = newElem; // Otherwise, set last element to new element

	list->currentLength++; // Increment the length
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, long data ) {
	if (list->activeElement == NULL) return;

	// Allocate memory for new element
	DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
	if (newElem == NULL) {
		DLL_Error();
		return;
	}

	// Set its data
	newElem->data = data;

	// Rewire pointers
	DLLElementPtr previousBackup = list->activeElement->previousElement;
	list->activeElement->previousElement = newElem;
	newElem->nextElement = list->activeElement;
	newElem->previousElement = previousBackup;

	// If there is a previous element, rewire its next pointer
	if (previousBackup != NULL) previousBackup->nextElement = newElem;
	else list->firstElement = newElem; // Otherwise, set first element to new element

	list->currentLength++; // Increment the length
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, long *dataPtr ) {
	if (list->activeElement == NULL) {
		DLL_Error();
		return;
	}

	// Set data
	*dataPtr = list->activeElement->data;
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, long data ) {
	if (list->activeElement == NULL) return;

	// Set data
	list->activeElement->data = data;
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
	if (list->activeElement == NULL) return;

	// Move to the next element
	list->activeElement = list->activeElement->nextElement;
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {
	if (list->activeElement == NULL) return;

	// Move to the previous element
	list->activeElement = list->activeElement->previousElement;
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive( DLList *list ) {
	return (list->activeElement != NULL);
}

/* Konec c206.c */
