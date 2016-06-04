/******************************************************************************/
/**           Projekt do predmetu Modelovani a simulace 2012/2013            **/
/******************************************************************************/
/**   Zadani:   Okruh 5 - SHO Logisticka firma                               **/
/**    Datum:   9. 12. 2012                                                  **/
/** Resitele:   Radim KUBIS, xkubis03                                        **/
/**             Radomir URBANEK, xurban41                                    **/
/******************************************************************************/
/**  Situace:   Bezny provoz.                                                **/
/******************************************************************************/
/**                   VSECHNY UVEDENE CASY JSOU V MINUTACH                   **/
/******************************************************************************/

#include <time.h>
#include <stdio.h>
#include "simlib.h"

/** Konstanty  ****************************************************************/

#define DELKA_SIMULACE 10*12*60

/* Pocty aut mezi pobockou a centralou */
#define POCET_AUT_USTI    2
#define POCET_AUT_LIBEREC 2
#define POCET_AUT_HRADEC  2
#define POCET_AUT_BRNO    8
#define POCET_AUT_PLZEN   3

/* Pocty ramp na pobockach a centrale */
#define POCET_RAMP_USTI    2
#define POCET_RAMP_LIBEREC 2
#define POCET_RAMP_HRADEC  2
#define POCET_RAMP_BRNO    7
#define POCET_RAMP_PLZEN   3

#define POCET_RAMP_CENTRALA  10

/* Pocty prepazek prijimajicich zasilky na pobockach */
#define PREPAZKY_USTI    2
#define PREPAZKY_LIBEREC 2
#define PREPAZKY_HRADEC  2
#define PREPAZKY_BRNO    4
#define PREPAZKY_PLZEN   2

/* Cinnosti na pobocce */
#define CESTA_DO_SKLADU 0.33  // 20s

#define INTERVAL_VAZENI_MIN 0.17  // 10s
#define INTERVAL_VAZENI_MAX 0.25  // 15s

#define INTERVAL_SKENOVANI_MIN 0.13  //  8s
#define INTERVAL_SKENOVANI_MAX 0.20  // 12s

#define ZADAVANI_DO_SYSTEMU_MIN 0.67  // 40s
#define ZADAVANI_DO_SYSTEMU_MAX 1.00  // 60s

/* Cile zasilek */
#define CIL_USTI    'U'
#define CIL_LIBEREC 'L'
#define CIL_HRADEC  'H'
#define CIL_BRNO    'B'
#define CIL_PLZEN   'P'

/* Kapacita auta */
#define KAPACITA_AUTA 95

/* Intervaly manipulace s baliky */
#define NALOZENI_MIN 0.25  // 15s
#define NALOZENI_MAX 0.42  // 25s

#define VYLOZENI_MIN 0.20  // 12s
#define VYLOZENI_MAX 0.33  // 20s

/* POCTY !!! prichodu zasilek na pobocky za hodinu */
#define PRICHODY_USTI    24.83
#define PRICHODY_LIBEREC 26.92
#define PRICHODY_HRADEC  24.67
#define PRICHODY_BRNO    99.83
#define PRICHODY_PLZEN   44.08

/* Intervaly delky jizdy mezi pobockou a centralou */
#define JIZDA_USTI_MIN  64
#define JIZDA_USTI_MAX 112

#define JIZDA_LIBEREC_MIN  73
#define JIZDA_LIBEREC_MAX 128

#define JIZDA_HRADEC_MIN  83
#define JIZDA_HRADEC_MAX 145

#define JIZDA_BRNO_MIN 119
#define JIZDA_BRNO_MAX 208

#define JIZDA_PLZEN_MIN  66
#define JIZDA_PLZEN_MAX 116

/* Hranice pravdepodobnosti vygenerovani zasilky z pobocky na jinou pobocku */
#define PST_Z_USTI_DO_LIBEREC 0.138
#define PST_Z_USTI_DO_HRADEC  0.126
#define PST_Z_USTI_DO_BRNO    0.510
#define PST_Z_USTI_DO_PLZEN   0.226

#define PST_Z_LIBEREC_DO_USTI   0.129
#define PST_Z_LIBEREC_DO_HRADEC 0.127
#define PST_Z_LIBEREC_DO_BRNO   0.516
#define PST_Z_LIBEREC_DO_PLZEN  0.228

#define PST_Z_HRADEC_DO_USTI    0.127
#define PST_Z_HRADEC_DO_LIBEREC 0.137
#define PST_Z_HRADEC_DO_BRNO    0.510
#define PST_Z_HRADEC_DO_PLZEN   0.226

#define PST_Z_BRNO_DO_USTI    0.206
#define PST_Z_BRNO_DO_LIBEREC 0.223
#define PST_Z_BRNO_DO_HRADEC  0.205
#define PST_Z_BRNO_DO_PLZEN   0.366

#define PST_Z_PLZEN_DO_USTI    0.141
#define PST_Z_PLZEN_DO_LIBEREC 0.153
#define PST_Z_PLZEN_DO_HRADEC  0.140
#define PST_Z_PLZEN_DO_BRNO    0.566

/** Pocty projetych cest mezi pobockami a centralou  **************************/
int pocetCestUsti    = 0;
int pocetCestLiberec = 0;
int pocetCestHradec  = 0;
int pocetCestBrno    = 0;
int pocetCestPlzen   = 0;

/** Histogramy  ***************************************************************/

Histogram dobaCesty("Celkova doba cesty zasilky v minutach", 120, 15, 90);

/** Sklady pro pobocky a centralu  ********************************************/

/* Sklady prichozich zasilek na jednotlivych pobockach */
Queue skladNaPobocceUsti("Sklad na pobocce Usti nad Labem");
Queue skladNaPobocceLiberec("Sklad na pobocce Liberec");
Queue skladNaPobocceHradec("Sklad na pobocce Hradec Kralove");
Queue skladNaPobocceBrno("Sklad na pobocce Brno");
Queue skladNaPoboccePlzen("Sklad na pobocce Plzen");

/* Sklady zasilek na centrale pro jednotlive pobocky */
Queue skladCentralyProUsti("Sklad na centrale pro Usti nad Labem");
Queue skladCentralyProLiberec("Sklad na centrale pro Liberec");
Queue skladCentralyProHradec("Sklad na centrale pro Hradec Kralove");
Queue skladCentralyProBrno("Sklad na centrale pro Brno");
Queue skladCentralyProPlzen("Sklad na centrale pro Plzen");

/** Rampy pobocek a centraly  *************************************************/

/* Rampy pro pobocky a centralu */
Store rampyUsti("Rampy Usti nad Labem", POCET_RAMP_USTI);
Store rampyLiberec("Rampy Liberec", POCET_RAMP_LIBEREC);
Store rampyHradec("Rampy Hradec Kralove", POCET_RAMP_HRADEC);
Store rampyBrno("Rampy Brno", POCET_RAMP_BRNO);
Store rampyPlzen("Rampy Plzen", POCET_RAMP_PLZEN);

Store rampyCentrala("Rampy centrala", POCET_RAMP_CENTRALA);

/** Prepazky pobocek  *********************************************************/

/* Prepazky pro pobocky */
Store prepazkyUsti("Prepazky Usti nad Labem", PREPAZKY_USTI);
Store prepazkyLiberec("Prepazky Liberec", PREPAZKY_LIBEREC);
Store prepazkyHradec("Prepazky Hradec Kralove", PREPAZKY_HRADEC);
Store prepazkyBrno("Prepazky Brno", PREPAZKY_BRNO);
Store prepazkyPlzen("Prepazky Plzen", PREPAZKY_PLZEN);

/** Zasilky pobocek  **********************************************************/

/* Proces zasilky */
class Zasilka : public Process {
  public:
    /* Cil, kam ma byt zasilka dorucena */
    int cil;
    /* Cas vzniku zasilky */
    int cas;
  
  /* Nastaveni cilove pobocky a casu vzniku zasilky */
  void nastavCil(int n) {
    cas = Time;
    cil = n;
  }

  /* Ziskani cilove pobocky */
  int dejCil() {
    return cil;
  }
};

/* Zasilka pobocky Usti */
class ZasilkaUsti : public Zasilka {

  /* Popis procesu zasilky pobocky Usti */
  void Behavior() {
/** Urceni cile zasilky  ******************************************************/
    /* Kam pujde zasilka? */
    double r = Random();
    
    if(r <= PST_Z_USTI_DO_LIBEREC) {
      // zasilka do Liberce
      nastavCil(CIL_LIBEREC);
    } else if(r <= (PST_Z_USTI_DO_LIBEREC+PST_Z_USTI_DO_HRADEC)) {
      // zasilka do Hradce
      nastavCil(CIL_HRADEC);
    } else if(r <= (PST_Z_USTI_DO_LIBEREC+PST_Z_USTI_DO_HRADEC+PST_Z_USTI_DO_BRNO)) {
      // zasilka do Brna
      nastavCil(CIL_BRNO);
    } else {
      // zasilka do Plzne
      nastavCil(CIL_PLZEN);
    }
/** Prace s novou zasilkou na pobocce  ****************************************/
    // Zaberu prepazku na pobocce pro novou zasilku
    Enter(prepazkyUsti);
    // Zvazim zasilku
    Wait(Uniform(INTERVAL_VAZENI_MIN, INTERVAL_VAZENI_MAX));
    // Zadavani do systemu
    Wait(Uniform(ZADAVANI_DO_SYSTEMU_MIN, ZADAVANI_DO_SYSTEMU_MAX));
    // Uvolnim prepazku
    Leave(prepazkyUsti);
    // Cesta do skladu
    Wait(CESTA_DO_SKLADU);
    // Vlozim zasilku do fronty pobocky
    skladNaPobocceUsti.Insert(this);
    // Uspim zasilku ve skladu pobocky, aby se mohl spustit v centrale
    Passivate();
/** Prace se zasilkou na centrale  ********************************************/
    // Oskenuji zasilku
    Wait(Uniform(INTERVAL_SKENOVANI_MIN, INTERVAL_SKENOVANI_MAX));
    // Cesta zasilky do skladu
    Wait(CESTA_DO_SKLADU);
    // Ulozim do skladu na centrale
    switch (cil) {
      case CIL_LIBEREC: {
        skladCentralyProLiberec.Insert(this);
        break;
      }
      case CIL_HRADEC: {
        skladCentralyProHradec.Insert(this);
        break;
      }
      case CIL_BRNO: {
        skladCentralyProBrno.Insert(this);
        break;
      }
      case CIL_PLZEN: {
        skladCentralyProPlzen.Insert(this);
        break;
      }
    }
    // Uspim ve skladu na centrale, aby se mohl probudit na cilove pobocce
    Passivate();
/** Prace se zasilkou na cilove pobocce  **************************************/
    // Zaznamenam dobu cesty
    dobaCesty(Time-cas);

  }
};

/* Zasilka pobocky Liberec */
class ZasilkaLiberec : public Zasilka {

  /* Popis procesu zasilky pobocky Liberec */
  void Behavior() {
/** Urceni cile zasilky  ******************************************************/
    /* Kam pujde zasilka? */
    double r = Random();
    
    if(r <= PST_Z_LIBEREC_DO_USTI) {
      // zasilka do Usti
      nastavCil(CIL_USTI);
    } else if(r <= (PST_Z_LIBEREC_DO_USTI+PST_Z_LIBEREC_DO_HRADEC)) {
      // zasilka do Hradce
      nastavCil(CIL_HRADEC);
    } else if(r <= (PST_Z_LIBEREC_DO_USTI+PST_Z_LIBEREC_DO_HRADEC+PST_Z_LIBEREC_DO_BRNO)) {
      // zasilka do Brna
      nastavCil(CIL_BRNO);
    } else {
      // zasilka do Plzne
      nastavCil(CIL_PLZEN);
    }
/** Prace s novou zasilkou na pobocce  ****************************************/
    // Zaberu prepazku na pobocce pro novou zasilku
    Enter(prepazkyLiberec);
    // Zvazim zasilku
    Wait(Uniform(INTERVAL_VAZENI_MIN, INTERVAL_VAZENI_MAX));
    // Zadavani do systemu
    Wait(Uniform(ZADAVANI_DO_SYSTEMU_MIN, ZADAVANI_DO_SYSTEMU_MAX));
    // Uvolnim prepazku
    Leave(prepazkyLiberec);
    // Cesta do skladu
    Wait(CESTA_DO_SKLADU);
    // Vlozim zasilku do fronty pobocky
    skladNaPobocceLiberec.Insert(this);
    // Uspim zasilku ve skladu pobocky, aby se mohl spustit v centrale
    Passivate();
/** Prace se zasilkou na centrale  ********************************************/
    // Oskenuji zasilku
    Wait(Uniform(INTERVAL_SKENOVANI_MIN, INTERVAL_SKENOVANI_MAX));
    // Cesta zasilky do skladu
    Wait(CESTA_DO_SKLADU);
    // Ulozim do skladu na centrale
    switch (cil) {
      case CIL_USTI: {
        skladCentralyProUsti.Insert(this);
        break;
      }
      case CIL_HRADEC: {
        skladCentralyProHradec.Insert(this);
        break;
      }
      case CIL_BRNO: {
        skladCentralyProBrno.Insert(this);
        break;
      }
      case CIL_PLZEN: {
        skladCentralyProPlzen.Insert(this);
        break;
      }
    }
    // Uspim ve skladu na centrale, aby se mohl probudit na cilove pobocce
    Passivate();
/** Prace se zasilkou na cilove pobocce  **************************************/
    // Zaznamenam dobu cesty
    dobaCesty(Time-cas);

  }
};

/* Zasilka pobocky Hradec */
class ZasilkaHradec : public Zasilka {

  /* Popis procesu zasilky pobocky Hradec */
  void Behavior() {
/** Urceni cile zasilky  ******************************************************/
    /* Kam pujde zasilka? */
    double r = Random();
    
    if(r <= PST_Z_HRADEC_DO_USTI) {
      // zasilka do Usti
      nastavCil(CIL_USTI);
    } else if(r <= (PST_Z_HRADEC_DO_USTI+PST_Z_HRADEC_DO_LIBEREC)) {
      // zasilka do Liberce
      nastavCil(CIL_LIBEREC);
    } else if(r <= (PST_Z_HRADEC_DO_USTI+PST_Z_HRADEC_DO_LIBEREC+PST_Z_HRADEC_DO_BRNO)) {
      // zasilka do Brna
      nastavCil(CIL_BRNO);
    } else {
      // zasilka do Plzne
      nastavCil(CIL_PLZEN);
    }
/** Prace s novou zasilkou na pobocce  ****************************************/
    // Zaberu prepazku na pobocce pro novou zasilku
    Enter(prepazkyHradec);
    // Zvazim zasilku
    Wait(Uniform(INTERVAL_VAZENI_MIN, INTERVAL_VAZENI_MAX));
    // Zadavani do systemu
    Wait(Uniform(ZADAVANI_DO_SYSTEMU_MIN, ZADAVANI_DO_SYSTEMU_MAX));
    // Uvolnim prepazku
    Leave(prepazkyHradec);
    // Cesta do skladu
    Wait(CESTA_DO_SKLADU);
    // Vlozim zasilku do fronty pobocky
    skladNaPobocceHradec.Insert(this);
    // Uspim zasilku ve skladu pobocky, aby se mohl spustit v centrale
    Passivate();
/** Prace se zasilkou na centrale  ********************************************/
    // Oskenuji zasilku
    Wait(Uniform(INTERVAL_SKENOVANI_MIN, INTERVAL_SKENOVANI_MAX));
    // Cesta zasilky do skladu
    Wait(CESTA_DO_SKLADU);
    // Ulozim do skladu na centrale
    switch (cil) {
      case CIL_USTI: {
        skladCentralyProUsti.Insert(this);
        break;
      }
      case CIL_LIBEREC: {
        skladCentralyProLiberec.Insert(this);
        break;
      }
      case CIL_BRNO: {
        skladCentralyProBrno.Insert(this);
        break;
      }
      case CIL_PLZEN: {
        skladCentralyProPlzen.Insert(this);
        break;
      }
    }
    // Uspim ve skladu na centrale, aby se mohl probudit na cilove pobocce
    Passivate();
/** Prace se zasilkou na cilove pobocce  **************************************/
    // Zaznamenam dobu cesty
    dobaCesty(Time-cas);

  }
};

/* Zasilka pobocky Brno */
class ZasilkaBrno : public Zasilka {

  /* Popis procesu zasilky pobocky Brno */
  void Behavior() {
/** Urceni cile zasilky  ******************************************************/
    /* Kam pujde zasilka? */
    double r = Random();
    
    if(r <= PST_Z_BRNO_DO_USTI) {
      // zasilka do Usti
      nastavCil(CIL_USTI);
    } else if(r <= (PST_Z_BRNO_DO_USTI+PST_Z_BRNO_DO_LIBEREC)) {
      // zasilka do Liberce
      nastavCil(CIL_LIBEREC);
    } else if(r <= (PST_Z_BRNO_DO_USTI+PST_Z_BRNO_DO_LIBEREC+PST_Z_BRNO_DO_HRADEC)) {
      // zasilka do Hradce
      nastavCil(CIL_HRADEC);
    } else {
      // zasilka do Plzne
      nastavCil(CIL_PLZEN);
    }
/** Prace s novou zasilkou na pobocce  ****************************************/
    // Zaberu prepazku na pobocce pro novou zasilku
    Enter(prepazkyBrno);
    // Zvazim zasilku
    Wait(Uniform(INTERVAL_VAZENI_MIN, INTERVAL_VAZENI_MAX));
    // Zadavani do systemu
    Wait(Uniform(ZADAVANI_DO_SYSTEMU_MIN, ZADAVANI_DO_SYSTEMU_MAX));
    // Uvolnim prepazku
    Leave(prepazkyBrno);
    // Cesta do skladu
    Wait(CESTA_DO_SKLADU);
    // Vlozim zasilku do fronty pobocky
    skladNaPobocceBrno.Insert(this);
    // Uspim zasilku ve skladu pobocky, aby se mohl spustit v centrale
    Passivate();
/** Prace se zasilkou na centrale  ********************************************/
    // Oskenuji zasilku
    Wait(Uniform(INTERVAL_SKENOVANI_MIN, INTERVAL_SKENOVANI_MAX));
    // Cesta zasilky do skladu
    Wait(CESTA_DO_SKLADU);
    // Ulozim do skladu na centrale
    switch (cil) {
      case CIL_USTI: {
        skladCentralyProUsti.Insert(this);
        break;
      }
      case CIL_LIBEREC: {
        skladCentralyProLiberec.Insert(this);
        break;
      }
      case CIL_HRADEC: {
        skladCentralyProHradec.Insert(this);
        break;
      }
      case CIL_PLZEN: {
        skladCentralyProPlzen.Insert(this);
        break;
      }
    }
    // Uspim ve skladu na centrale, aby se mohl probudit na cilove pobocce
    Passivate();
/** Prace se zasilkou na cilove pobocce  **************************************/    
    // Zaznamenam dobu cesty
    dobaCesty(Time-cas);

  }
};

/* Zasilka pobocky Plzen */
class ZasilkaPlzen : public Zasilka {

  /* Popis procesu zasilky pobocky Plzen */
  void Behavior() {
/** Urceni cile zasilky  ******************************************************/
    /* Kam pujde zasilka? */
    double r = Random();
    
    if(r <= PST_Z_PLZEN_DO_USTI) {
      // zasilka do Usti
      nastavCil(CIL_USTI);
    } else if(r <= (PST_Z_PLZEN_DO_USTI+PST_Z_PLZEN_DO_LIBEREC)) {
      // zasilka do Liberce
      nastavCil(CIL_LIBEREC);
    } else if(r <= (PST_Z_PLZEN_DO_USTI+PST_Z_PLZEN_DO_LIBEREC+PST_Z_PLZEN_DO_HRADEC)) {
      // zasilka do Hradce
      nastavCil(CIL_HRADEC);
    } else {
      // zasilka do Brna
      nastavCil(CIL_BRNO);
    }
/** Prace s novou zasilkou na pobocce  ****************************************/
    // Zaberu prepazku na pobocce pro novou zasilku
    Enter(prepazkyPlzen);
    // Zvazim zasilku
    Wait(Uniform(INTERVAL_VAZENI_MIN, INTERVAL_VAZENI_MAX));
    // Zadavani do systemu
    Wait(Uniform(ZADAVANI_DO_SYSTEMU_MIN, ZADAVANI_DO_SYSTEMU_MAX));
    // Uvolnim prepazku
    Leave(prepazkyPlzen);
    // Cesta do skladu
    Wait(CESTA_DO_SKLADU);
    // Vlozim zasilku do fronty pobocky
    skladNaPoboccePlzen.Insert(this);
    // Uspim zasilku ve skladu pobocky, aby se mohl spustit v centrale
    Passivate();
/** Prace se zasilkou na centrale  ********************************************/
    // Oskenuji zasilku
    Wait(Uniform(INTERVAL_SKENOVANI_MIN, INTERVAL_SKENOVANI_MAX));
    // Cesta zasilky do skladu
    Wait(CESTA_DO_SKLADU);
    // Ulozim do skladu na centrale
    switch (cil) {
      case CIL_USTI: {
        skladCentralyProUsti.Insert(this);
        break;
      }
      case CIL_LIBEREC: {
        skladCentralyProLiberec.Insert(this);
        break;
      }
      case CIL_HRADEC: {
        skladCentralyProHradec.Insert(this);
        break;
      }
      case CIL_BRNO: {
        skladCentralyProBrno.Insert(this);
        break;
      }
    }
    // Uspim ve skladu na centrale, aby se mohl probudit na cilove pobocce
    Passivate();
/** Prace se zasilkou na cilove pobocce  **************************************/    
    // Zaznamenam dobu cesty
    dobaCesty(Time-cas);

  }
};

/** Generatory zasilek  *******************************************************/

/* Generator prichodu zasilek na pobocku Usti */
class GeneratorUsti : public Event {
  void Behavior() {
    /* Vznik nove zasilky */
    (new ZasilkaUsti)->Activate();
    /* Naplanovani vzniku dalsi zasilky */
    Activate(Time+Exponential(60.0/PRICHODY_USTI));
  }
};

/* Generator prichodu zasilek na pobocku Liberec */
class GeneratorLiberec : public Event {
  void Behavior() {
    /* Vznik nove zasilky */
    (new ZasilkaLiberec)->Activate();
    /* Naplanovani vzniku dalsi zasilky */
    Activate(Time+Exponential(60.0/PRICHODY_LIBEREC));
  }
};

/* Generator prichodu zasilek na pobocku Hradec */
class GeneratorHradec : public Event {
  void Behavior() {
    /* Vznik nove zasilky */
    (new ZasilkaHradec)->Activate();
    /* Naplanovani vzniku dalsi zasilky */
    Activate(Time+Exponential(60.0/PRICHODY_HRADEC));
  }
};

/* Generator prichodu zasilek na pobocku Brno */
class GeneratorBrno : public Event {
  void Behavior() {
    /* Vznik nove zasilky */
    (new ZasilkaBrno)->Activate();
    /* Naplanovani vzniku dalsi zasilky */
    Activate(Time+Exponential(60.0/PRICHODY_BRNO));
  }
};

/* Generator prichodu zasilek na pobocku Plzen */
class GeneratorPlzen : public Event {
  void Behavior() {
    /* Vznik nove zasilky */
    (new ZasilkaPlzen)->Activate();
    /* Naplanovani vzniku dalsi zasilky */
    Activate(Time+Exponential(60.0/PRICHODY_PLZEN));
  }
};

/** Nakladni auta  ************************************************************/

/* Proces auta pro pobocky */
class Auto : public Process {
  public:
    /* Fronta baliku v aute */
    Queue obsahAuta;
  
  /* Destruktor */
  ~Auto() {
    /* Tiskne statistiky obsazenosti auta */
    obsahAuta.Output();
  }
};

/* Proces auta pobocky Usti */
class AutoUsti : public Auto {

  /* Popis procesu auta pobocky Usti */
  void Behavior() {
    // Pomocny ukazatel pro prekladani zasilky
    Entity *z = NULL;
    
    // Nastaveni nazvu fronty auta
    obsahAuta.SetName("Auto Usti nad Labem");
    
    // Auto jezdi stale dokola
    while(1) {
      // Jedu na pobocku
      Wait(Uniform(JIZDA_USTI_MIN, JIZDA_USTI_MAX));
/** Cinnost auta na pobocce  **************************************************/
      // Obsadim rampu pobocky
      Enter(rampyUsti);
      pocetCestUsti++;

      // Vylozim baliky, pokud mam
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nalozim baliky - do kapacity
      //                A/NEBO
      //                - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladNaPobocceUsti.Empty()) {
        // Vezmu balik
        z = skladNaPobocceUsti.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu pobocky
      Leave(rampyUsti);

      // Jedu na centralu
      Wait(Uniform(JIZDA_USTI_MIN, JIZDA_USTI_MAX));
/** Cinnost auta na centrale  *************************************************/
      // Obsadim rampu centraly
      Enter(rampyCentrala);
      pocetCestUsti++;
      
      // Vykladam baliky, pokud jsou
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nakladam baliky - do kapacity
      //                 A/NEBO
      //                 - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladCentralyProUsti.Empty()) {
        z = skladCentralyProUsti.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu centraly
      Leave(rampyCentrala);
      
      // PROCES opet pokracuje od zacatku ;)
    }
  }
};

/* Proces auta pobocky Liberec */
class AutoLiberec : public Auto {

  /* Popis procesu auta pobocky Liberec */
  void Behavior() {
    // Pomocny ukazatel pro prekladani zasilky
    Entity *z = NULL;
    
    // Nastaveni nazvu fronty auta
    obsahAuta.SetName("Auto Liberec");
    
    // Auto jezdi stale dokola
    while(1) {
      // Jedu na pobocku
      Wait(Uniform(JIZDA_LIBEREC_MIN, JIZDA_LIBEREC_MAX));
/** Cinnost auta na pobocce  **************************************************/
      // Obsadim rampu pobocky
      Enter(rampyLiberec);
      pocetCestLiberec++;
      
      // Vylozim baliky, pokud mam
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nalozim baliky - do kapacity
      //                A/NEBO
      //                - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladNaPobocceLiberec.Empty()) {
        // Vezmu balik
        z = skladNaPobocceLiberec.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu pobocky
      Leave(rampyLiberec);
      
      // Jedu na centralu
      Wait(Uniform(JIZDA_LIBEREC_MIN, JIZDA_LIBEREC_MAX));
/** Cinnost auta na centrale  *************************************************/
      // Obsadim rampu centraly
      Enter(rampyCentrala);
      pocetCestLiberec++;
      
      // Vykladam baliky, pokud jsou
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nakladam baliky - do kapacity
      //                 A/NEBO
      //                 - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladCentralyProLiberec.Empty()) {
        z = skladCentralyProLiberec.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu centraly
      Leave(rampyCentrala);
      
      // PROCES opet pokracuje od zacatku ;)
    }
  }
};

/* Proces auta pobocky Hradec */
class AutoHradec : public Auto {

  /* Popis procesu auta pobocky Hradec */
  void Behavior() {
    // Pomocny ukazatel pro prekladani zasilky
    Entity *z = NULL;
    
    // Nastaveni nazvu fronty auta
    obsahAuta.SetName("Auto Hradec Kralove");
    
    // Auto jezdi stale dokola
    while(1) {
      // Jedu na pobocku
      Wait(Uniform(JIZDA_HRADEC_MIN, JIZDA_HRADEC_MAX));
/** Cinnost auta na pobocce  **************************************************/
      // Obsadim rampu pobocky
      Enter(rampyHradec);
      pocetCestHradec++;
      
      // Vylozim baliky, pokud mam
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nalozim baliky - do kapacity
      //                A/NEBO
      //                - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladNaPobocceHradec.Empty()) {
        // Vezmu balik
        z = skladNaPobocceHradec.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu pobocky
      Leave(rampyHradec);
      
      // Jedu na centralu
      Wait(Uniform(JIZDA_HRADEC_MIN, JIZDA_HRADEC_MAX));
/** Cinnost auta na centrale  *************************************************/
      // Obsadim rampu centraly
      Enter(rampyCentrala);
      pocetCestHradec++;
      
      // Vykladam baliky, pokud jsou
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nakladam baliky - do kapacity
      //                 A/NEBO
      //                 - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladCentralyProHradec.Empty()) {
        z = skladCentralyProHradec.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu centraly
      Leave(rampyCentrala);
      
      // PROCES opet pokracuje od zacatku ;)
    }
  }
};

/* Proces auta pobocky Brno */
class AutoBrno : public Auto {

  /* Popis procesu auta pobocky Brno */
  void Behavior() {
    // Pomocny ukazatel pro prekladani zasilky
    Entity *z = NULL;
    
    // Nastaveni nazvu fronty auta
    obsahAuta.SetName("Auto Brno");
    
    // Auto jezdi stale dokola
    while(1) {
      // Jedu na pobocku
      Wait(Uniform(JIZDA_BRNO_MIN, JIZDA_BRNO_MAX));
/** Cinnost auta na pobocce  **************************************************/
      // Obsadim rampu pobocky
      Enter(rampyBrno);
      pocetCestBrno++;
      
      // Vylozim baliky, pokud mam
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nalozim baliky - do kapacity
      //                A/NEBO
      //                - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladNaPobocceBrno.Empty()) {
        // Vezmu balik
        z = skladNaPobocceBrno.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu pobocky
      Leave(rampyBrno);
      
      // Jedu na centralu
      Wait(Uniform(JIZDA_BRNO_MIN, JIZDA_BRNO_MAX));
/** Cinnost auta na centrale  *************************************************/
      // Obsadim rampu centraly
      Enter(rampyCentrala);
      pocetCestBrno++;
      
      // Vykladam baliky, pokud jsou
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nakladam baliky - do kapacity
      //                 A/NEBO
      //                 - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladCentralyProBrno.Empty()) {
        z = skladCentralyProBrno.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu centraly
      Leave(rampyCentrala);
      
      // PROCES opet pokracuje od zacatku ;)
    }
  }
};

/* Proces auta pobocky Plzen */
class AutoPlzen : public Auto {

  /* Popis procesu auta pobocky Plzen */
  void Behavior() {
    // Pomocny ukazatel pro prekladani zasilky
    Entity *z = NULL;
    
    // Nastaveni nazvu fronty auta
    obsahAuta.SetName("Auto Plzen");
    
    // Auto jezdi stale dokola
    while(1) {
      // Jedu na pobocku
      Wait(Uniform(JIZDA_PLZEN_MIN, JIZDA_PLZEN_MAX));
/** Cinnost auta na pobocce  **************************************************/
      // Obsadim rampu pobocky
      Enter(rampyPlzen);
      pocetCestPlzen++;
      
      // Vylozim baliky, pokud mam
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nalozim baliky - do kapacity
      //                A/NEBO
      //                - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladNaPoboccePlzen.Empty()) {
        // Vezmu balik
        z = skladNaPoboccePlzen.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu pobocky
      Leave(rampyPlzen);
      
      // Jedu na centralu
      Wait(Uniform(JIZDA_PLZEN_MIN, JIZDA_PLZEN_MAX));
/** Cinnost auta na centrale  *************************************************/
      // Obsadim rampu centraly
      Enter(rampyCentrala);
      pocetCestPlzen++;
      
      // Vykladam baliky, pokud jsou
      while(!obsahAuta.Empty()) {
        // Vezmu balik
        z = obsahAuta.GetLast();
        // Cekam na vylozeni baliku
        Wait(Uniform(VYLOZENI_MIN, VYLOZENI_MAX));
        // Zasilka je vylozena
        ((Zasilka*)z)->Activate();
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Nakladam baliky - do kapacity
      //                 A/NEBO
      //                 - dokud nejake mam
      while(obsahAuta.Length() < KAPACITA_AUTA && !skladCentralyProPlzen.Empty()) {
        // Vezmu balik
        z = skladCentralyProPlzen.GetFirst();
        // Cekam na nalozeni baliku
        Wait(Uniform(NALOZENI_MIN, NALOZENI_MAX));
        // Nalozim balik
        obsahAuta.Insert(z);
        // Uvolnim se pro dalsi balik
        z = NULL;
      }
      
      // Uvolnim rampu centraly
      Leave(rampyCentrala);
      
      // PROCES opet pokracuje od zacatku ;)
    }
  }
};

/** Program simulace **********************************************************/

/* Hlavni program */
int main()
{
  /* Pomocna promenna pro cykly */
  int i = 0;
  
  /* Nastaveni vystupu */
  SetOutput("output_projekt_bezny_provoz.dat");

  /* Inicializace pseudonahodneho generatoru */
  RandomSeed(time(NULL));

  /* Inicializace delky simulace */
  Init(0, DELKA_SIMULACE);
  
  /* Inicializace generatoru prichodu zasilek na pobocky */
  (new GeneratorUsti)->Activate();
  (new GeneratorLiberec)->Activate();
  (new GeneratorHradec)->Activate();
  (new GeneratorBrno)->Activate();
  (new GeneratorPlzen)->Activate();
  
  /* Vytvorim auta pro pobocky */
  for(i = 0; i < POCET_AUT_USTI; i++) {
    (new AutoUsti)->Activate();
  }
  for(i = 0; i < POCET_AUT_LIBEREC; i++) {
    (new AutoLiberec)->Activate();
  }
  for(i = 0; i < POCET_AUT_HRADEC; i++) {
    (new AutoHradec)->Activate();
  }
  for(i = 0; i < POCET_AUT_BRNO; i++) {
    (new AutoBrno)->Activate();
  }
  for(i = 0; i < POCET_AUT_PLZEN; i++) {
    (new AutoPlzen)->Activate();
  }
  
  /* Spusteni simulace */
  Run();
  
  /* Statistiky doby cekani zasilek na pobocce */
  skladNaPobocceUsti.Output();
  skladNaPobocceLiberec.Output();
  skladNaPobocceHradec.Output();
  skladNaPobocceBrno.Output();
  skladNaPoboccePlzen.Output();
  
  /* Statistiky doby cekani zasilek na centrale */
  skladCentralyProUsti.Output();
  skladCentralyProLiberec.Output();
  skladCentralyProHradec.Output();
  skladCentralyProBrno.Output();
  skladCentralyProPlzen.Output();
  
  /* Statistiky doby cekani zasilek pred prepazkou */
  prepazkyUsti.Output();
  prepazkyLiberec.Output();
  prepazkyHradec.Output();
  prepazkyBrno.Output();
  prepazkyPlzen.Output();
  
  /* Statistiky vyuziti ramp */
  rampyUsti.Output();
  rampyLiberec.Output();
  rampyHradec.Output();
  rampyBrno.Output();
  rampyPlzen.Output();
  
  rampyCentrala.Output();
  
  /* Statistiky stravene doby zasilek v systemu */
  dobaCesty.Output();
 
  /* 
  printf("Pocet jizd Usti: %d\n", pocetCestUsti);
  printf("Pocet jizd Liberec: %d\n", pocetCestLiberec);
  printf("Pocet jizd Hradec: %d\n", pocetCestHradec);
  printf("Pocet jizd Brno: %d\n", pocetCestBrno);
  printf("Pocet jizd Plzen: %d\n", pocetCestPlzen);
  */
}
