/***************************************************************************//**
  @file     userDatabase.h
  @brief    Manejo de la base de datos de acceso
  @author   Grupo 5
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include "const.h"
#include "event_queue/event_queue.h"
#include "userDatabase.h"

#define MAXUSER			100


/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

typedef struct{
    uint8_t id[IDSIZE];
    uint8_t password[PASSMAX];
    uint8_t admin;
    uint8_t floor;
    bool inside;
} user;

static uint16_t indice;
static uint16_t usercount;
static user database[MAXUSER];
bool userType;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

uint8_t searchUser(uint8_t * id);
void copyUser();
uint8_t passwordEquals(uint8_t* password,uint8_t indice);
bool sameid(uint8_t* id, uint8_t* del_id);
void updateListDis(uint8_t* id);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void init_Database(){
	database[0] = (user){.id={4,5,1,7,6,6,0,1}, .password={0,1,2,3,NULLCHAR}, .admin=0, .floor=1, .inside=false};    // Usuarios de prueba
	database[1] = (user){.id={4,4,5,4,7,0,7,1}, .password={5,4,3,2,1}, .admin=1, .floor=1, .inside=false}; // ITBA Sergio
	database[2] = (user){.id={3,4,5,9,5,7,2,8}, .password={4,5,6,7,8}, .admin=1, .floor=2, .inside=false}; 
	database[3] = (user){.id={2,2,5,0,1,0,4,8}, .password={0,7,0,3,NULLCHAR}, .admin=0, .floor=2, .inside=false};    // Galeno 1
	database[4] = (user){.id={2,2,5,0,3,0,5,1}, .password={0,0,0,0,NULLCHAR}, .admin=0, .floor=3, .inside=false};    // Galeno 2
   	database[5] = (user){.id={4,4,5,4,8,7,1,5}, .password={5,4,3,2,1}, .admin=1, .floor=3, .inside=false}; // ITBA Basili
	usercount = 6;
}

void list_init(){
    indice=0;
    updateListDis(database[indice].id);
}

void up_menu_del(){
    if(indice>0){
        indice--;
        updateListDis(database[indice].id);
    }
}

void down_menu_del(){
    if(indice<usercount-1){
        indice++;
        updateListDis(database[indice].id);
    }
} 

void internal_del_user(uint8_t* id){
    if(usercount>0 && !sameid(id, database[indice].id)){
        copyUser(id);
        usercount--;
        add_event(BACK);
    }
}

void copyUser(){
    for(int i=0; i<IDSIZE; i++){
        ((database[indice]).id)[i]=((database[usercount-1]).id)[i];
    }

    for(int i=0; i<PASSMAX; i++){
        ((database[indice]).password)[i]=((database[usercount-1]).password)[i];
    }
}

bool internal_check_id(uint8_t * id){
    bool found = false;
    bool nullID = false;
    for (uint8_t i = 0; i < IDSIZE && !nullID; i++){
        if(id[i] == NULLCHAR)
            nullID = true;
    }
    
    if(!nullID){
        for (uint8_t i = 0; i < usercount && !found; i++){
            found = true;
            for (uint8_t j = 0; j < IDSIZE; j++){
                if(database[i].id[j] != id[j])
                    found = false;
            }
        }
    }
    return found;
}

bool internal_check_pass(uint8_t * id, uint8_t * password){
    
    // Verifico que la password 
    for (uint8_t i = 0; i < PASSMIN; i++){
        if(password[i] == NULLCHAR)
            return false;
    }

    uint8_t indice = searchUser(id);
    if (indice==MAXUSER)
        return false;
    return passwordEquals(password, indice);
}

uint8_t searchUser(uint8_t * id){
    uint8_t equal=true;

    for (uint8_t user=0; user<usercount; user++){
        equal=true;

        for (uint8_t digit=0; digit<IDSIZE && equal; digit++){
            if(id[digit] != ((database[user]).id)[digit]){
                equal=false;
            }
            else if(digit==IDSIZE-1)
                return user;
        }
    }
    return MAXUSER;
}

uint8_t passwordEquals(uint8_t* password,uint8_t indice){
    for(uint8_t digit=0; digit<PASSMAX; digit++){
        if(password[digit] != ((database[indice]).password)[digit])
            return false;
    }
    return true; 
}
   

bool internal_used_id(uint8_t * id){
    if(searchUser(id)!=MAXUSER) // Verifico si el usuario existe
        return true;

    return false;
}

bool internal_save_pass(uint8_t * id, uint8_t * pass){
    uint8_t indice=searchUser(id);
    if (indice>=MAXUSER){
        return false;
    }

    for (uint8_t i = 0; i < PASSMIN; i++){
        if(pass[i] == NULLCHAR)
            return false;
    }

    for (uint8_t i = 0; i < PASSMAX; i++){
        (database[indice]).password[i] = pass[i];
    }
    return true;
}
   
bool internal_add_user(uint8_t * id, uint8_t * pass){
    if(usercount >= MAXUSER)
        return false;

    else{
        for (uint8_t i = 0; i < IDSIZE; i++){
            (database[usercount]).id[i] = id[i];
        }

        for (uint8_t i = 0; i < PASSMAX; i++){
            (database[usercount]).password[i] = pass[i];
        }

        (database[usercount]).admin=userType;
        usercount++;
    }
    return true;
}

bool isAdmin(uint8_t* id){
	 return database[searchUser(id)].admin;
}

bool sameid(uint8_t* id, uint8_t* del_id){
    for(uint8_t i=0; i<IDSIZE; i++){
        if(id[i]!=del_id[i])
            return false;
    }
    return true;
}

bool internal_verifyPass(uint8_t* password){
    for (uint8_t i = 0; i < PASSMIN; i++){
        if(password[i] == NULLCHAR)
            return false;
    }
    return true;
}

bool avaliableUsers(){
    if(usercount>=MAXUSER)
        return false;
    else
        return true;
}

void toggleUser(){
    userType = !userType;
}

uint8_t actualType(){
    if(userType){
        return ADMIN;
    }
    else 
        return NORMAL;
}

uint8_t getFloor(uint8_t* id){
    uint8_t indice=searchUser(id);
    if (indice>=MAXUSER){
        return false;
    }

    return database[indice].floor;
}

bool isInside(uint8_t* id){
    uint8_t indice=searchUser(id);
    if (indice>=MAXUSER){
        return false;
    }

    return database[indice].inside;
}

bool setInside(uint8_t* id, bool value){
    uint8_t indice=searchUser(id);
    if (indice>=MAXUSER){
        return false;
    }

    database[indice].inside=value;
    return true;
}

/*******************************************************************************
 ******************************************************************************/
