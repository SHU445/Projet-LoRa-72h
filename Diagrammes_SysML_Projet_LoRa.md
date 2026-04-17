# Diagrammes SysML - Projet LoRa 72h

Regrouper diagrammes projet:
- smartphone avec application de messagerie Bluetooth,
- balise LoRa basee sur ESP32-S3 LoRa + Bluetooth,
- systeme d'alternance d'antennes (omnidirectionnelle / directionnelle),
- Arduino Uno pour balayage et rotation de l'antenne directionnelle.

## 1) Diagramme de cas d'utilisation

```mermaid
flowchart LR
  rU["👤 Utilisateur"]:::role
  rAPP["👤 << service>> Application mobile"]:::role
  rBT["👤 << service>> Balise LoRa ESP32-S3 + Bluetooth"]:::role
  rARD["👤 << service>> Arduino Uno (rotation/balayage)"]:::role
  rANT["👤 Systeme d'antennes (omni + directionnelle)"]:::role

  subgraph S["Systeme de communication LoRa/Bluetooth"]
    direction TB
    ucConnect([Se connecter a la balise])
    ucSend([Envoyer un message])
    ucReceive([Recevoir un message])
    ucStatus([Consulter le statut de connexion])
    ucData([Consulter des donnees de la balise])
    ucSelectAntenna([Selectionner le type d'antenne])
    ucScan([Balayer / orienter l'antenne directionnelle])
    
    ucSend -. include .-> ucConnect
    ucReceive -. include .-> ucConnect
    ucData -. include .-> ucConnect
    ucSelectAntenna -. include .-> ucData
    ucScan -. include .-> ucSelectAntenna
  end

  rU --- ucConnect
  rU --- ucSend
  rU --- ucReceive
  rU --- ucStatus
  rU --- ucData
  rU --- ucSelectAntenna

  ucConnect --- rAPP
  ucSend --- rAPP
  ucReceive --- rAPP
  ucStatus --- rAPP
  ucData --- rAPP

  ucConnect --- rBT
  ucSend --- rBT
  ucReceive --- rBT
  ucStatus --- rBT
  ucData --- rBT
  ucSelectAntenna --- rBT
  ucScan --- rBT

  ucScan --- rARD
  ucScan --- rANT

  classDef role stroke-width:0px;
```

## 2) Diagramme des exigences

```mermaid
flowchart TD
    EX0[EX-00 Systeme de communication LoRa/Bluetooth]

    EX1[EX-01 Communication BT smartphone <-> balise]
    EX2[EX-02 Envoi de messages texte]
    EX3[EX-03 Reception de messages texte]
    EX4[EX-04 Affichage de l'etat de connexion]
    EX5[EX-05 Disponibilite mode ecoute/visibilite]
    EX6[EX-06 Consultation de donnees statut balise]
    EX7[EX-07 Selection antenne omni/directionnelle]
    EX8[EX-08 Balayage + rotation par Arduino Uno]
    EX9[EX-09 Fonctionnement embarque robuste]

    EX0 --> EX1
    EX0 --> EX2
    EX0 --> EX3
    EX0 --> EX4
    EX0 --> EX5
    EX0 --> EX6
    EX0 --> EX7
    EX0 --> EX8
    EX0 --> EX9

    V1[(Validation: test appairage BT)]
    V2[(Validation: test message app -> balise)]
    V3[(Validation: test reception balise -> app)]
    V4[(Validation: test etats NOT_CONNECTED/LISTENING/CONNECTING/CONNECTED)]
    V5[(Validation: test alternance antennes)]
    V6[(Validation: test rotation/balayage)]

    EX1 -.verifiee par.-> V1
    EX2 -.verifiee par.-> V2
    EX3 -.verifiee par.-> V3
    EX4 -.verifiee par.-> V4
    EX7 -.verifiee par.-> V5
    EX8 -.verifiee par.-> V6
```

## 3) Diagramme de sequence (envoi d'un message)

```mermaid
sequenceDiagram
    actor U as Utilisateur
    participant APP as Application mobile
    participant CTRL as ConnectionController
    participant BT as Bluetooth HC-05/ESP32
    participant ARD as Arduino Uno

    U->>APP: Saisit puis envoie un message
    APP->>CTRL: sendMessage(message)
    CTRL->>BT: write(message + "\\n")
    BT->>ARD: Transmet trame serie Bluetooth
    ARD->>ARD: Lit buffer jusqu'a fin '\\n'
    ARD-->>U: Affiche "Recu: <message>" (moniteur serie + LED)
```

## 4) Diagramme de blocs (BDD)

```mermaid
classDiagram
    class SystemeLoRa72h {
      +communiquer()
      +superviserConnexion()
      +selectionnerAntenne()
    }

    class Smartphone {
      +LoRaApp
      +BluetoothAdapter
    }

    class Balise {
      +ESP32S3_LoRa_BT
      +ModuleBluetooth
      +SystemeAlternanceAntenne
    }

    class ArduinoUno {
      +balayage()
      +rotation()
    }

    class AntenneOmni
    class AntenneDirectionnelle

    SystemeLoRa72h *-- Smartphone
    SystemeLoRa72h *-- Balise
    SystemeLoRa72h *-- ArduinoUno
    Balise *-- AntenneOmni
    Balise *-- AntenneDirectionnelle
    Balise --> ArduinoUno : commande orientation
    Smartphone --> Balise : liaison Bluetooth
```

## 5) Diagramme de bloc interne (IBD simplifie)

```mermaid
flowchart LR
    subgraph S[Smartphone]
        UI[UI Chat]
        SVC[Service BT]
        UI <--> SVC
    end

    subgraph B[Balise]
        ESP[ESP32-S3 LoRa + BT]
        SW[Commutateur d'antenne]
        AO[Antenne omni]
        AD[Antenne directionnelle]
        ESP --> SW
        SW --> AO
        SW --> AD
    end

    subgraph A[Arduino Uno]
        MOT[Pilotage rotation]
        SCAN[Balayage]
    end

    SVC <--> ESP
    ESP <--> MOT
    MOT --> SCAN
    SCAN --> AD
```

## 6) Diagramme d'etat (connexion Bluetooth)

```mermaid
stateDiagram-v2
    [*] --> NOT_CONNECTED

    NOT_CONNECTED --> LISTENING: prepareForAccept()
    NOT_CONNECTED --> CONNECTING: connect(device)

    LISTENING --> CONNECTED: connexion entrante acceptee
    CONNECTING --> CONNECTED: socket connectee

    CONNECTING --> NOT_CONNECTED: echec connexion
    CONNECTED --> NOT_CONNECTED: disconnect()/connectionLost()
    LISTENING --> NOT_CONNECTED: stop()/cancel()
```


RAVAVA JE VAIS ME TROUER LE DERCHE