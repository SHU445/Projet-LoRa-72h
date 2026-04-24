# Diagrammes SysML - Projet LoRa 72h

Regrouper diagrammes projet:
- smartphone avec application de messagerie Bluetooth,
- balise basee sur ESP32-S3 LoRa,
- Arduino Uno pour balayage et rotation,
- systeme d'alternance entre antenne directionnelle et omnidirectionnelle,
- alimentation batterie pour les sous-systemes embarques.

## 1) Diagramme de cas d'utilisation

```mermaid
flowchart LR
  rU["Utilisateur"]:::role
  rAPP["<< service>>\nApplication mobile"]:::role
  rESP["<< service>>\nESP32-S3 LoRa"]:::role
  rARD["<< service>>\nArduino Uno"]:::role
  rConnect["<< service>>\nSe connecter a la balise Bluetoot03"]::role

  subgraph S["Balise LoRa"]
    direction TB
    ucSend([Envoyer un message texte])
    ucReceive([Recevoir un message texte])
    ucStatus([Consulter l'etat du lien])
    ucData([Consulter des donnees balise])
    ucSelectAntenna([Choisir antenne Omni/Dir])
    ucScan([Lancer balayage])

    ucSend -. include .-> rcConnect
    ucReceive -. include .-> rcConnect
    ucStatus -. include .-> rcConnect
    ucData -. include .-> rcConnect
    ucSelectAntenna -. include .-> ucData
    ucScan -. include .-> ucSelectAntenna
  end

  rU --- rAPP
  rAPP --- rconnect
  rconnect --- rESP

  ucSend --- rESP
  uStatus --rESP
  ucData --- rESP
  ucSelectAntenna --- rESP


  ucScan --- rARD

  classDef role stroke-width:0px;
```

## 2) Diagramme des exigences

```mermaid
flowchart TD
    EX0[EX-00 Systeme balise LoRa communicante]

    EX1[EX-01 Communication Bluetooth smartphone <-> balise]
    EX2[EX-02 Echange de messages texte bidirectionnel]
    EX3[EX-03 Affichage des etats de connexion]
    EX4[EX-04 Consultation de donnees de la balise]
    EX5[EX-05 Choix antenne directionnelle ou omnidirectionnelle]
    EX6[EX-06 Balayage/rotation assures par Arduino Uno]
    EX7[EX-07 Alimentation autonome par batterie]
    EX8[EX-08 Architecture robuste en environnement exterieur]

    EX0 --> EX1
    EX0 --> EX2
    EX0 --> EX3
    EX0 --> EX4
    EX0 --> EX5
    EX0 --> EX6
    EX0 --> EX7
    EX0 --> EX8

    V1[(Validation: test appairage BT)]
    V2[(Validation: test envoi/rception messages)]
    V3[(Validation: test etats NOT_CONNECTED/LISTENING/CONNECTING/CONNECTED)]
    V4[(Validation: test consultation donnees balise)]
    V5[(Validation: test alternance antennes)]
    V6[(Validation: test rotation/balayage Arduino)]
    V7[(Validation: test autonomie batterie)]

    EX1 -.verifiee par.-> V1
    EX2 -.verifiee par.-> V2
    EX3 -.verifiee par.-> V3
    EX4 -.verifiee par.-> V4
    EX5 -.verifiee par.-> V5
    EX6 -.verifiee par.-> V6
    EX7 -.verifiee par.-> V7
```

## 3) Diagramme de sequence (envoi d'un message)

```mermaid
sequenceDiagram
    actor U as Utilisateur
    participant APP as Application mobile
    participant ESP as ESP32-S3 LoRa (balise)
    participant ARD as Arduino Uno (rotation)
    participant SW as Systeme d'alternance
    participant ANT as Antenne choisie

    U->>APP: Choisit antenne + saisit message
    APP->>ESP: Trame Bluetooth (message + commande)
    ESP->>SW: Balayage du signal
    SW->>ANT: Active antenne Dir
    ARD->>ANT: Rotation / orientation
    ESP-->>APP: Ack + statut liaison + donnees
    APP-->>U: Affichage message et statut
```

## 4) Diagramme de blocs (BDD)

```mermaid
classDiagram
    class SystemeLoRa72h {
      +communiquer()
      +superviserConnexion()
      +selectionnerAntenne()
      +alimenterSousSystemes()
    }

    class Smartphone {
      +LoRaApp
      +BluetoothAdapter
    }

    class Balise {
      +ESP32S3_LoRa
      +Bluetooth
    }

    class ArduinoUno {
      +balayage()
      +rotation()
    }

    class SystemeAlternanceAntennes
    class SystemeRotationAntennes
    class AntenneOmni
    class AntenneDirectionnelle
    class Batterie

    SystemeLoRa72h *-- Smartphone
    SystemeLoRa72h *-- Balise
    SystemeLoRa72h *-- ArduinoUno
    SystemeLoRa72h *-- SystemeAlternanceAntennes
    SystemeLoRa72h *-- SystemeRotationAntennes
    SystemeLoRa72h *-- Batterie
    SystemeAlternanceAntennes *-- AntenneOmni
    SystemeAlternanceAntennes *-- AntenneDirectionnelle
    Smartphone --> Balise : liaison Bluetooth
    Balise --> ArduinoUno : commandes
    Balise --> SystemeAlternanceAntennes
    ArduinoUno --> SystemeRotationAntennes
```

## 5) Diagramme de bloc interne (IBD simplifie)

```mermaid
flowchart LR
    subgraph S[Smartphone]
        UI[UI Chat]
        SVC[Service BT]
        UI <--> SVC
    end

    subgraph B[Balise/Embarque]
        ESP[ESP32-S3 LoRa]
        BT[Bluetooth]
        ALT[Systeme d'alternance des antennes]
        ROT[Systeme rotation des antennes]
        AO[Antenne Omni]
        AD[Antenne Dir]
        BAT[Batterie]
        ESP <--> BT
        ESP --> ALT
        ALT --> AO
        ALT --> AD
        ESP --> ROT
        ROT --> AD
        BAT --> ESP
        BAT --> ALT
        BAT --> ROT
    end

    subgraph A[Arduino Uno]
        MOT[Pilotage rotation]
        SCAN[Balayage]
        BAT2[Alim depuis batterie]
        BAT2 --> MOT
        BAT2 --> SCAN
    end

    SVC <--> BT
    ESP <--> MOT
    MOT --> SCAN
    SCAN --> AO
```

## 6) Diagramme d'etat (connexion Bluetooth)

```mermaid
stateDiagram-v2
    [*] --> INIT
    INIT --> NOT_CONNECTED: demarrage service BT

    NOT_CONNECTED --> LISTENING: mode visible / prepareForAccept()
    NOT_CONNECTED --> CONNECTING: tentative connexion sortante

    LISTENING --> CONNECTED: connexion entrante acceptee
    CONNECTING --> CONNECTED: appairage + socket OK
    CONNECTED --> ANTENNA_SELECTED: commande choix antenne
    ANTENNA_SELECTED --> SCANNING: antenne directionnelle + balayage
    ANTENNA_SELECTED --> CONNECTED: antenne omni
    SCANNING --> CONNECTED: orientation terminee

    CONNECTING --> NOT_CONNECTED: echec connexion
    CONNECTED --> NOT_CONNECTED: perte lien / deconnexion
    LISTENING --> NOT_CONNECTED: annulation
    NOT_CONNECTED --> [*]: arret systeme
```

## 7) Schema de fonctionnement global

```mermaid
flowchart TD
    U[Utilisateur] --> APP[Application mobile]
    APP -->|Bluetooth| ESP[Balise ESP32-S3 LoRa]

    ESP --> ALT[Systeme d'alternance]
    ALT --> DECIDE{Antenne choisie ?}
    DECIDE -->|Omnidirectionnelle| AO[Antenne Omni]
    DECIDE -->|Directionnelle| ALT
    ALT --> AD[Antenne Directionnelle]
    ESP --> ARD[Arduino Uno]
    ARD --> ROT[Rotation / Balayage]
    ROT --> AD

    AO --> TX[Transmission / Reception radio]
    AD --> TX

    TX --> ESP
    ESP -->|Ack + etat liaison + donnees| APP
    APP --> AFF[Affichage message, statut, donnees]
    AFF --> U

    BAT[Batterie] --> ESP
    BAT --> ALT
    BAT --> ARD
```