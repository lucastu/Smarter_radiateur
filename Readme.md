Commanade de radiateur via fil pilote avec arduino

based on  https://www.instructables.com/E-Paper-Display-Partial-Refresh-With-Arduino/
I wired the display to the Arduino Nano as follows:

Display <-> Arduino
BUSY <-> D7
RST <-> D8
DC <-> D9
CS <-> D10
DIN <-> D11
CLK <-> D13
VCC <-> 3.3V
GND <-> GND

if heatingProgramState = 0  // Mode attente								.
	if bouton 1 long													.
		=> fonction Changer heure										.
	if bouton 1 court													.
		=> activer chauffe programmée à 5h (heatingProgramState)		.
	if bouton 2 court													.
		=> Activer chauffe now  1h (heatingProgramState  =2)			.
																		.
if heatingProgramState = 1  // mode chauffe programmée					.
	if bouton 1 court 													.
		=> repousser chauffe programmée à + 1h
	if bouton 2 court 													.
		=> Fonction stop												.
																		.
	if !nextday && heure passe à 0										.
		=> nextday = true												.
	if nextday 															.
	    if heure start < heure  && heure >	heure Stop					.
			=> activer chauffage										.
		if heure >	heure Stop											.
			=> Fonction stop											.
																		.
if heatingProgramState = 2 // mode chauffe now							.
	if bouton 1 court													.
		=> ajouter 15 minutes avec 3h max								.
	if bouton 2 court 													.
		=> Fonction stop												.
	if heure >	heure Stop												.
		=> Fonction stop												.

Fonction stop															.
	Retourne en heatingProgramState 0
	coupe le chauffage
	Réaffiche les icones de base
