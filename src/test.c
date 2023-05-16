#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int freq_to_note(int freq){
    /*Fonction en escalier qui convertit les fréquences en note (numéro de touche du piano)*/

if (freq < 28){
return 0;
}
else{
if (freq <30){
return 1;
}
else{
if (freq < 31){
return 2;
}
else{
if (freq < 33){
return 3;
}
else{
if (freq < 35){
return 4;
}
else{
if (freq < 37){
return 5;
}
else{
if (freq < 39){
return 6;
}
else{
if (freq < 42){
return 7;
}
else{
if (freq < 44){
return 8;
}
else{
if (freq < 47){
return 9;
}
else{
if (freq < 49){
return 10;
}
else {
if (freq < 52){
return 11;
}
else{
if (freq < 56){
return 12;
}
else {
if (freq < 59){
return 13;
}
else{
if (freq < 62){
return 14;
}
else{
if (freq < 66){
return 15;
}
else{
if (freq < 70){
return 16;
}
else {
if (freq < 74){
return 17;
}
else{
if (freq < 78){
return 18;
}
else{
if (freq < 83){
return 20;
}
else{

if (freq < 88){
return 21;
}
else {
if (freq < 93){
return 22;
}
else{
if (freq < 104){
return 23;
}
else{
if (freq < 111){
return 24;
}
else{
if (freq < 117){
return 25;
}
else{
if (freq < 124){
return 26;
}
else {
if (freq < 131){
return 27;
}
else {
if (freq < 139){
return 28;
}
else {
if (freq < 147){
return 29;
}
else{
if (freq < 160){
return 30;
}
else{
if (freq < 169){
return 31;
}
else{
if (freq < 179){
return 32;
}
else{
if (freq < 189){
return 33;
}
else{
if (freq < 200){
return 34;
}
else{
if (freq < 215){
return 35;
}
else{
if (freq < 228){
return 36;
}
else{
if (freq < 240){
return 37;
}
else{
if (freq < 255){
return 38;
}
else{
if (freq < 269){
return 39;
}
else{
if (freq < 284){
return 40;
}
else{
if (freq < 302){
return 41;
}
else{
if (freq < 320){
return 42;
}
else{
if (freq < 340){
return 43;
}
else{
if (freq < 355){
return 44;
}
else{
if (freq < 385){
return 45;
}
else{
if (freq < 405){
return 46;
}
else{
if (freq < 430){
return 47;
}
else{
if (freq < 450){
return 48;
}
else{
if (freq < 480){
return 49;
}
else{
if (freq < 510){
return 50;
}
else{
if (freq < 540){
return 51;
}
else {
if (freq < 565){
return 52;
}
else {
if (freq < 605){
return 53;
}
else{
if (freq < 640){
return 54;
}
else{
if (freq < 675){
return 55;
}
else{
if (freq < 720){
return 56;
}
else{
if (freq < 760){
return 57;
}
else{
if (freq < 805){
return 58;
}
else {
if (freq < 850){
return 59;
}
else{
if (freq < 900){
return 60;
}
else{
if (freq < 955){
return 61;
}
else{
if (freq < 1020){
return 62;
}
else{
if (freq < 1075){
return 63;
}
else{
if (freq < 1130){
return 64;
}
else{
if (freq < 1205){
return 65;
}
else{
if (freq < 1285){
return 66;
}
else{
if (freq < 1355){
return 67;
}
else{
if (freq < 1435){
return 68;
}
else{
if (freq < 1515){
return 69;
}
else{
if (freq < 1610){
return 70;
}
else{
if (freq < 1710){
return 71;
}
else{
if (freq < 1810){
return 72;
}
else{
if (freq < 1920){
return 73;
}
else{
if (freq < 2015){
return 74;
}
else{
if (freq < 2150){
return 75;
}
else{
if (freq < 2290){
return 76;
}
else{
if (freq < 2400){
return 77;
}
else{
if (freq < 2575){
return 78;
}
else{
if (freq < 2700){
return 79;
}
else{
if (freq < 2850){
return 80;
}
else{
if (freq < 3050){
return 81;
}
else{
if(freq < 3240){
return 82;
}
else{
if(freq < 3450){
return 83;
}
else{
if (freq < 3650){
return 84;
}
else{
if (freq < 3840){
return 85;
}
else{
if (freq < 4050){
    return 86;
}
else{
    return 87;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}


int main(){

    int freq = 283;
    int val = freq_to_note(freq);

    printf("freq : %d , note : %d\n", freq, val);


    return 0;
}