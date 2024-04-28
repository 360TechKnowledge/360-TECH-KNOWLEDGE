// const int num_known_uids = 3;  // Number of known RFID tag UIDs
// const char known_uids[num_known_uids][11] = {
//     "02004CBB57",  // Example known RFID UID 1
//     "0200658A92",  // Example known RFID UID 2
//     "02005A3FD3"   // Example known RFID UID 3
// };

// // Function to check if an RFID tag UID is in the array of known UIDs
// bool isUIDInArray(const char* uid) {
//     for (int i = 0; i < num_known_uids; i++) {  // Loop through known UIDs
//         if (strcmp(uid, known_uids[i]) == 0) {  // If the UIDs match
//             return true;  // The UID is known
//         }
//     }
//     return false;  // If no match is found, the UID is unknown
// }

// if (rdm6300.available() >= 14) {  // Ensure we have a complete RFID tag read
//         char uid[11];  // Array to store the RFID UID (10 characters + null terminator)
//         rdm6300.read();  // Discard the start byte
//         for (int i = 0; i < 10; i++) {
//             uid[i] = rdm6300.read();  // Read the 10-character UID
//         }
//         uid[10] = '\0';  // Null-terminate the string
//         rdm6300.read();  // Discard the end byte
        
//         Serial.print("RFID Tag Read: ");  // Output the UID
//         Serial.println(uid);
        
//         if (isUIDInArray(uid)) {  // Check if the UID is known
//             Serial.println("Access Granted!");  // If it's known
//         } else {
//             Serial.println("Access Denied.");  // If it's unknown
//         }
//     }

//     delay(500);  // Delay to avoid rapid re-checking