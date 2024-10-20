/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 * 
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Pokud fronta s odpovídající prioritou neexistuje, tato metoda ji alokuje
 * a inicializuje. Za jejich korektní uvolnení odpovídá volající.
 * 
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet( DLList *packetLists, PacketPtr packet ) {
	bool priorityFound = false;
	QosPacketListPtr priorityListPtr;
	
	// Check if list with given priority is iniciliazed
	DLL_First(packetLists);
	while (DLL_IsActive(packetLists)) {
		priorityListPtr = (QosPacketListPtr)packetLists->activeElement->data;

		if (priorityListPtr->priority == packet->priority) {
			priorityFound = true;
			break;
		}
		
		DLL_Next(packetLists);
	}
	
	// If not, initialize it
	if (!priorityFound) {
		priorityListPtr = malloc(sizeof(QosPacketList));
		if (priorityListPtr == NULL) {
			error_flag = true;
			return;
		}

		priorityListPtr->list = malloc(sizeof(DLList));
		if (priorityListPtr->list == NULL) {
			error_flag = true;
			return;
		}

		priorityListPtr->priority = packet->priority;
		DLL_InsertFirst(packetLists, (long)priorityListPtr);
		DLL_Init(priorityListPtr->list);
	}

	// Insert packet address to the list
	DLL_InsertFirst(priorityListPtr->list, (long)packet);

	// If list exceeds the maximum packet count, remove every second packet
	if (priorityListPtr->list->currentLength > MAX_PACKET_COUNT) {
		DLL_First(priorityListPtr->list);

		while (DLL_IsActive(priorityListPtr->list)) {
			DLL_DeleteAfter(priorityListPtr->list);
			DLL_Next(priorityListPtr->list);
		}
	}
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 * 
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 * 
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets( DLList *packetLists, DLList *outputPacketList, int maxPacketCount ) {
	int packetSentCount = 0;

	// Send packets by priority until max packet count is reached or no packets are left
	while (packetSentCount <= maxPacketCount) {
		DLL_First(packetLists);
		int highestPriority = -(__INT8_MAX__);
		QosPacketListPtr highestPriorityList = (QosPacketListPtr)packetLists->activeElement->data;
		QosPacketListPtr currentPriorityList = (QosPacketListPtr)packetLists->activeElement->data;

		// Find the highest priority packet list, non-empty if possible
		while (DLL_IsActive(packetLists)) {
			currentPriorityList = (QosPacketListPtr)packetLists->activeElement->data;

			if (currentPriorityList->priority >= highestPriority && 
				currentPriorityList->list->currentLength != 0) 
			{
				highestPriorityList = currentPriorityList;
				highestPriority = highestPriorityList->priority;
			}
				
			DLL_Next(packetLists);
		}

		// If no packets are left or packetList is empty, break
		if (highestPriorityList == NULL || highestPriorityList->list->currentLength == 0) break;

		// Send the highest priority packet
		DLL_InsertLast(outputPacketList, (long)highestPriorityList->list->lastElement->data);

		// And remove it from the list
		DLL_DeleteLast(highestPriorityList->list);
		
		packetSentCount++;
	}
}
