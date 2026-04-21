# Szamitogep_Grafika_2026
# Virtuális Kertész Szimulátor

Ez a projekt egy interaktív, 3D-s grafikai alkalmazás, amely egy beltéri zöldházat szimulál. A program az **OpenGL** és **SDL2** könyvtárakra épül.

---


## Minimális követelmények

* **Kamerakezelés:** A virtuális teret az egér és/vagy billentyűzet segítségével be kell tudni járni.
* **Objektumok:** Térbeli objektumok legyenek. Ezeket, mint modelleket külön fájlból lehessen betölteni.
* **Animáció, mozgatás:** A program legyen interaktív. A modelleket, fényeket billentyűzettel, egérrel lehessen mozgatni; legyenek animált részek.
* **Textúrák:** A modelleknek legyen textúrája.
* **Fények:** A programban a fényeket a + és - billentyűk segítségével lehessen állítani (vagy egyéb kézenfekvő módon a program leírása alapján).
* **Használati útmutató:** Az F1 gomb megnyomására (vagy egyéb kézenfekvő módon) jelenjen megy egy leírás a programról, annak használatáról, kezelési módjáról.

---

## Teljesített többlet funkciók

* **Átlátszóság:** Átlátszó részeket tartalmazó textúrák alkalmazása, félig áttetsző részek a megjelenített objektumokon.
* **Ütközésvizsgálat:** Bounding-box implementáció valamilyen formája.
* **Objektumok kijelölése egérrel:** A megjelenített objektumok kijelölése.
* **Részecskerendszer:** Dinamikus öntözőrendszer, amelynél a vízcseppek fizikája (gyorsulás, esés) és élettartama valós időben frissül a cserepek felett.
* **Köd:** Dinamikusan változó sűrűségű köd-effekt, amely az öntözéssel párhuzamosan fokozatosan jelenik meg és tűnik el.
* **Procedurális geometria:** A falak és a padló automatikus felosztása apró négyszögekre (Tesselláció) a fényhatás javítása érdekében.

---

## Irányítás és Keybindok

A szimulátor az alábbi billentyűkkel és interakciókkal vezérelhető:

### Kamera és Mozgás
* **`W`, `A`, `S`, `D`**: Mozgás a 3D térben.
* **Egér mozgatása**: Nézelődés, a kamera irányítása.

### Interakció
* **`Egér Bal Klikk`**: Cserép kijelölése (a kijelölt cserép piros fénnyel dereng, adatai megjelennek a HUD-on).
* **`1`**: **Kaktusz** ültetése a kijelölt cserépbe.
* **`2`**: **Gyom** ültetése a kijelölt cserépbe.
* **`0`**: Cserép kiürítése.

### Speciális Funkciók
* **`R`**: **Öntözőrendszer bekapcsolása**. 10 másodpercig tartó esőeffekt és fokozatosan sűrűsödő pára/köd.
* **`+,-`**: Fényerő állítása.
* **`F1`**: Súgó (Help) képernyő megjelenítése.
* **`ESC`**: Kilépés a programból.

---

## Technikai részletek

* **Nyelv:** C
* **Grafikai API:** OpenGL 1.2
* **Ablak- és eseménykezelés:** SDL2
* **Betűtípusok és HUD:** FreeGLUT

---

## Fordítás és Futtatás

A projekt fordítása a mellékelt `Makefile` segítségével történik:

1.  Nyisd meg a terminált a projekt gyökérmappájában.
2.  Töltsd le az asseteket a következő linken: https://mega.nz/folder/r7IwUBKC#zRd5AnZx5Hh7EvLUOE8vWQ
3.  Fordítsd le a projektet:
    ```bash
    make
    ```
4.  Futtasd az alkalmazást:
    ```bash
    ./Program.exe
    ```

---

## Felhasznált források és eszközök

A projekt során felhasznált külső erőforrások jegyzéke:

### 3D Modellek és textúrák

* **Potted plant:** https://www.cgtrader.com/items/6879084/download-page
* **Empty pot:** https://www.cgtrader.com/items/132855/download-page
* **Plant_a:** https://models.spriters-resource.com/gamecube/takthepowerofjuju/asset/463322/
* **Plant_b:** https://models.spriters-resource.com/gamecube/takthepowerofjuju/asset/463181/
* **Floor:** https://www.alldesigncreative.com/3d-textures/bathroom-tile-textures/
* **Walls** https://www.renderhub.com/radju/mossy-brick-wall-pbr-material
* **Roof** https://mbdecor.co.uk/tag/new-ultimo-and-mineral-collection/

A projekt kizárólag oktatási célból készült a **Miskolci Egyetem Számítógépes Grafika** kurzusára. Minden felhasznált külső tartalom a szerzői jogok tiszteletben tartásával, ingyenes licencek keretében került felhasználásra.
