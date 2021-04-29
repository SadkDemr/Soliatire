#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <cstdlib>

struct Card_Veriler{
	char color;
	char suit;
	int deger; // De�erler = 1 i�in A, 11 i�in J, 12 i�in Q, 13 i�in K
	bool dogrumu; // Acik i�in do�ru, Kapali i�in yanl��
	Card_Veriler *next;
};

struct List_Veriler{
	int listno;
	List_Veriler *next;
	Card_Veriler *kartlar;
};

struct List{
	Card_Veriler *ust_baslik;//�st liste ba�l���
	List_Veriler *oyunsonu_baslik;
	List_Veriler *tahta_baslik;
	FILE *fptr;
	
	void create();// Toplist, tahtaListeleri ve oyunSonuListeleri olu�turmay� i�erir
	void create_toplist();
	void create_tahtaListeleri();
	void create_oyunSonuListeleri();
	void printkart(Card_Veriler *kart);
	void printlisteler();// B�t�n listeleri yazd�r�r
	void printmenu();//Se�im men�s�
	bool listeyeKartEkle(List_Veriler *secilenListe, Card_Veriler *transferKarti, int hangilistetipi);//Aktar�lan kart� se�ilen listeye ekler, hangi liste t�r� program taraf�ndan girilir
	void goster_ToplisttenDizmeye();//Girdi al�r ve ilgili i�lev �a�r�lar�
	void ToplisttenDizmeye(char s_color, char s_suit, int s_deger);
	void goster_ToplisttenTahtaya();
	void ToplisttenTahtaya(int listeIndexi, char s_color, char s_suit, int s_deger);
	void goster_TahtadanTahtaya();
	void TahtadanTahtaya(int nereden, int nereye, char s_color, char s_suit, int s_deger);//kartlar� tahta �zerinde hareket ettirir
	void goster_TahtadanDizmeye();//girdilerle ve �a�r�larla ilgili i�lev
	void TahtadanDizmeye(int nereden);//tahtadaki kart� oyun sonu dizimine ekler
	void islem_yap();//yap�lacak i�lem sevilir
	bool ToplistBosmu, tahtaBosmu;// oyun sonu kontrol i�in gerekli
	void clear_screen();//ekran temizleme fonksiyonu
	void close();//kapatma fonksiyonu
};

List islem;
using namespace std;

int main()
{
	char secim;
	islem.create();
	if(!islem.fptr)//Solitaire.txt dosyas�n� bulamazsan�z, program� kapat�n
	return 0;

	islem.ToplistBosmu= false;//�st liste bo�sa, bu de�er do�ru olacakt�r
	islem.tahtaBosmu = false;//Pano listesi bo�sa, bu de�er do�ru olacakt�r

	while(1){
		islem.printlisteler();
		islem.printmenu();
		cin >> secim;
		cin.ignore(10000, '\n');

		if(secim == '1'){
			islem.goster_ToplisttenDizmeye();
			cin.ignore(10000, '\n');
			getchar();
		}
		else if(secim == '2'){
			islem.goster_ToplisttenTahtaya();
			cin.ignore(10000, '\n');
			getchar();
		}
		else if(secim == '3'){
			islem.goster_TahtadanTahtaya();
			cin.ignore(10000, '\n');
			getchar();
		}
		else if(secim == '4'){
			islem.goster_TahtadanDizmeye();
			cin.ignore(10000, '\n');
		}
		else if(secim == '9'){
			islem.close();
			printf("\n\nOyun Kapatilir ve tum kartlar bellekten silinir. Cikmak icin enter tusuna basiniz.");
			cin.ignore(10000, '\n');
			getchar();
			break;
		}
		else
			cout <<"Gecersiz secim" <<endl;

		if(!islem.ust_baslik)// �st listeyi kontrol et veya bo� b�rak
			islem.ToplistBosmu= true;	
		List_Veriler *capraz;
		capraz = islem.tahta_baslik;
		int sayac = 0;
		while(capraz){//pano listelerini kontrol etmek bo� ya da de�il
			if(!capraz->kartlar)
				sayac++;
			capraz = capraz->next;
			if(sayac == 7)//7 tahta listesinin t�m� bo�sa saya� 7'dir
				islem.tahtaBosmu = true;
		}

		/*Oyun tamamland���nda kullan�c�ya mesaj yazd�r�r*/
		if(islem.tahtaBosmu && islem.ToplistBosmu){//Pano ve �st listeler bo�ken d�ng� s�ras�nda dur
			printf("\n\n\n\t\t\tOYUNU KAZANDINIZ!!!! OYUN BITTI! TEBRIKLER!\n");// oyun bitti mesaj�
			printf("\t\t\tOYUNU KAZANDINIZ!!!! OYUN BITTI! TEBRIKLER!\n");
			printf("\nTum kartlar bellekten silinir. Cikmak icin enter tusuna basiniz.");
			cin.ignore(1000, '\n');
			islem.close();
			getchar();
			break;
		}
	}
	
	return 0;
}

void List::create(){
	fptr = fopen("solitaire.txt", "r+");
	if(!fptr){
		cout << "'solitaire.txt' dosyas� bulunamad�.Program kapat�lacak."<< endl;
		getchar();
		return;
	}
	fseek(fptr, 0, SEEK_SET);
	create_toplist();
	create_tahtaListeleri();
	create_oyunSonuListeleri();
}//fonk. sonu

void List::create_toplist(){
	Card_Veriler *newnode, *final;
	char gecicicolor, gecicisuit, gecicideger[4], gecicidogrumu[4], garbage[10];//
	ust_baslik = NULL;

	while(1){
		newnode = new Card_Veriler;
		newnode->next = NULL;
		fscanf(fptr, "%c", &gecicicolor);
		if(gecicicolor == '*'){//ilk y�ld�z okuma
				fscanf(fptr, "%6c", garbage);// di�er be� y�ld�z� ge�er ve yeni �izgiye gider 
				break;
		}
		else
			newnode->color = gecicicolor;

		fscanf(fptr, " %c %s %s ", &gecicisuit, gecicideger, gecicidogrumu);
		newnode->suit = gecicisuit;
		
		/*Char karakterler int de�erlere aktar�l�yor*/
		if(gecicideger[0] == 'A')	newnode->deger = 1;
		else if(gecicideger[0] == 'J')newnode->deger = 11;
		else if(gecicideger[0] == 'Q')newnode->deger = 12;
		else if(gecicideger[0] == 'K')newnode->deger = 13;
		else 
			sscanf(gecicideger, "%d", &newnode->deger);
		if(strcmp(gecicidogrumu, "Acik") == 0)
			newnode->dogrumu = true;
		if(strcmp(gecicidogrumu, "Kapali") == 0)
			newnode->dogrumu = false;
		if(ust_baslik == NULL){//ilk d���m� bo� top liste ekleme
			ust_baslik = newnode;
			final = ust_baslik;
		}
		else{//d���m� listeye ekleme
			final->next = newnode;
			final = final->next;
		}
	}//while sonu
}

void List::create_tahtaListeleri(){
	Card_Veriler *newnode, *final;//kart listesinin son d���m�ne son noktalar
	List_Veriler *yeniTahtaListesi, *tahtaListesiFinal;// tahtaListesiFinal puanlar�n�n son d���m�ne son noktalar
	char gecicicolor, gecicisuit, gecicideger[4], gecicidogrumu[8], garbage[10];
	int index = 1;// Bu dizin pano listesini temsil eder
	yeniTahtaListesi = new List_Veriler;// ilk tahta listesini yarat�r
	tahta_baslik = yeniTahtaListesi;
	tahtaListesiFinal = yeniTahtaListesi;
	yeniTahtaListesi->listno = index++;
	yeniTahtaListesi->next = NULL;
	yeniTahtaListesi->kartlar = NULL;

	while(!feof(fptr)){//bu while d�ng�s� tahta multi linked listelerini olu�turur		
	newnode = new Card_Veriler;
		newnode->next = NULL;
		fscanf(fptr, "%c", &gecicicolor);
		if(gecicicolor == '*'){//ilk y�ld�z okuma
				fscanf(fptr, "%6c", garbage);//di�er be� y�ld�z� ge�er ve yeni �izgiye gider
				yeniTahtaListesi = new List_Veriler;
				yeniTahtaListesi->listno = index++;
				yeniTahtaListesi->next = NULL;
				yeniTahtaListesi->kartlar = NULL;
				tahtaListesiFinal->next = yeniTahtaListesi;
				tahtaListesiFinal = yeniTahtaListesi;
				continue;
		}
		else
			newnode->color = gecicicolor;

		fscanf(fptr, " %c %s %s ", &gecicisuit, gecicideger, gecicidogrumu);
		newnode->suit = gecicisuit;
		if(gecicideger[0] == 'A')	newnode->deger = 1;
		else if(gecicideger[0] == 'J')newnode->deger = 11;
		else if(gecicideger[0] == 'Q')newnode->deger = 12;
		else if(gecicideger[0] == 'K')newnode->deger = 13;
		else 
			sscanf(gecicideger, "%d", &newnode->deger);

		if(strcmp(gecicidogrumu, "Acik") == 0)
			newnode->dogrumu = true;
		if(strcmp(gecicidogrumu, "Kapali") == 0)
			newnode->dogrumu = false;

		if(tahtaListesiFinal->kartlar == NULL){//ilk d���m� bo� tahta listesine ekle
			tahtaListesiFinal->kartlar = newnode;
			final = newnode;
		}
		else{//bo� tahta listesine ekle
			final->next = newnode;
			final = final->next;
		}
	}//while sonu
}

void List::create_oyunSonuListeleri(){
	oyunsonu_baslik = NULL;
	List_Veriler *yeniListe, *finalListe;
	int index = 1;//Ma�a liste indexi = 1, Kupa liste indexi = 2, Karo liste indexi = 3, Sinek liste indexi = 4

	/*Ma�a Listesini ba�latma*/
	yeniListe = new List_Veriler;
	yeniListe->kartlar = NULL;
	yeniListe->next = NULL;
	yeniListe->listno = index++;
	oyunsonu_baslik = yeniListe;
	finalListe = yeniListe;

	/*Di�er 3 listeyi ba�latma*/
	for(int i = 0 ; i <3; i++)	{
		yeniListe = new List_Veriler;
		yeniListe->kartlar = NULL;
		yeniListe->next = NULL;
		yeniListe->listno = index++;
		finalListe->next = yeniListe;
		finalListe = yeniListe;
	}//for sonu
}// fonksiyon sonu

void List::printkart(Card_Veriler *kart){// kart d���m�n�n verilerini yazd�r�r
	if(!kart->dogrumu ){//Kapal� ise kart� gizle
		printf("X");
		return;
	}
	char a;
	if(kart->deger == 1) a='A';
	else if(kart->deger == 11) a='J';
	else if(kart->deger == 12) a='Q';
	else if(kart->deger == 13) a='K';
	else a = '\0';

	if(!a)printf("%c,%c,%d", kart->color, kart->suit, kart->deger);
	else printf("%c,%c,%c", kart->color, kart->suit, a);
}

void List::printlisteler(){
	clear_screen();
	Card_Veriler *ct[7];//Kart listesi kart d���m�  ; ct[0] 1.liste i�in, ct[1] 2.liste i�in ....
	Card_Veriler *foundct[4];//Bulunan Liste Kart� D���m, foundct[0] = Ma�alar, foundct[1] = Kupalar, foundct[2] = Karolar, foundct[3] = Sinekler 
	Card_Veriler *topct;// TopList Kard taray�c�
	List_Veriler *listeGecisi;// Liste D���m� Gezgini


	cout << "Ust (Top) Listesi:" << endl;
	topct = ust_baslik;
	while(topct){
		printkart(topct);
		printf("|");
		topct = topct->next;
	}

	cout << endl << "\nTahta Listeleri:" << endl;
	for(int i=1; i <8; i++)
		cout << i << ".Liste\t";
	cout <<endl;

	listeGecisi = tahta_baslik;
	for(int i = 0; i < 7; i++){// ct[] pointerlerinin ba�lat�lmas�
		ct[i] = listeGecisi->kartlar;
		listeGecisi = listeGecisi->next;
	}

	/*Tahta listelerindeki kartlar�n yazd�r�lmaz�*/
	for(int i = 0; i < 19; i++){//d�ng� i�in listeler aras�nda ge�i� yapar ve 19 kez a�a�� iner, bir liste 19'a kadar kart i�erebilir (6 a�a�� kart + 13 yukar� kart) 
		for(int j = 0; j < 7; j++)
		{
			if(ct[j]){//ct [j] bo� de�ilse, yazd�r�n ve sonraki d���me gidin
				printkart(ct[j]);
				printf("\t");
				ct[j] = ct[j]->next;
			}
			else//ct [j] bo�sa, bir sekme yazd�r�n
				printf("\t");
		}
		if(ct[0] || ct[1] || ct[2] || ct[3] || ct[4] || ct[5] || ct[6])// bir hat yazd�rd�ktan sonra; 
			printf("\n");// en az bir kart bo� de�ilse: yeni sat�ra git
			else
			break;// sat�rdaki t�m kartlar null ise: loop i�in d��tan ay�r
	}//d�� i�in

	cout << endl << "Oyun sonu Listeleri:" << endl;
	cout << "Maca\tKupa\tKaro\tSinek" << endl;

	listeGecisi = oyunsonu_baslik;
	for(int i = 0; i < 4; i++){//foundct[] pointerlerinin ba�lat�lmas�
		foundct[i] = listeGecisi->kartlar;
		listeGecisi = listeGecisi->next;
	}

	for(int i = 0; i < 13; i++){//d�ng� i�in temel listelerini ge�er (en fazla 13 kart bir temel listesinde olabilir)
		for(int j = 0; j < 4; j++)
		{
			if(foundct[j]){//ct [j] bo� de�ilse, yazd�r�n ve sonraki d���me gidin
				printkart(foundct[j]);
				printf("\t");
				foundct[j] = foundct[j]->next;
			}
			else//foundct [j] bo�sa, bir sekme yazd�r�n
				printf("\t");
		}
		if(foundct[0] || foundct[1] || foundct[2] || foundct[3])//bir line yazd�ktan sonra; 
			printf("\n");//en az bir kart bo� de�ilse: yeni sat�ra git
		else
			break;//sat�rdaki t�m kartlar null ise: loop i�in d��tan ay�r
	}//d�� i�in
	printf("\n\n");
}//fonksiyon sonu
void List::printmenu(){
	cout << "Bir islem secin:" << endl;
	cout << "\t1. Top listesinden Oyun sonu listesi aktarim icin" << endl;
	cout << "\t2. Top listesinden Tahtaya aktarim icin" << endl;
	cout << "\t3. Tahta uzerinde hareket (degisim) icin" << endl;
	cout << "\t4. Tahtadan Oyun sonu listesine aktarim icin" << endl;
	cout << "Lutfen secim yapiniz (1, 2, 3, ya da 4), (Cikis icin 9'a basiniz.):" ;
}
bool List::listeyeKartEkle(List_Veriler *secilenListe, Card_Veriler *transferKarti, int hangilistetipi){// hangi liste t�r� kullan�c� ile ilgili de�il, program taraf�ndan otomatik olarak girilir
	Card_Veriler *kartCaprazlama;
	kartCaprazlama= secilenListe->kartlar;
	if(hangilistetipi == 1){//hangilistetipi 1 olmas� i�in tahtada
		if(kartCaprazlama == NULL && transferKarti->deger == 13){//liste bo�sa kart�n de�eri K = 13 olmal�d�r
			secilenListe->kartlar = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(!kartCaprazlama)//kart ge�i�i burada bo� olamaz, sadece K = 13 de�eri i�in bo� olabilir
			return false;
		while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;

		if(!kartCaprazlama->dogrumu){//kart kapal� ise, iki kart aras�ndaki renk ve de�er �nemli de�ildir			
kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(kartCaprazlama->dogrumu)// kart tak�l�ysa, iki kart aras�ndaki renk ve de�er �nemlidir		
		if(!(transferKarti->color == kartCaprazlama->color))// iki biti�ik kart aras�ndaki renkler farkl�ysa				
if(kartCaprazlama->deger - transferKarti->deger == 1 ){// Son de�eri listele - transfer kartlar� de�eri - 1 olmal�d�r
			kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
	}

	if(hangilistetipi == 2){// hangilistetipi 2 dir ��nk� top listten oyun sonuna ta��mak i�in.
		if(kartCaprazlama == NULL && transferKarti->deger == 1){// Liste bo�sa kart�n de�eri A = 1 olmal�d�r
			secilenListe->kartlar = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(!kartCaprazlama)// kart ge�idi burada bo� olamaz, sadece A = 1 de�eri i�in bo� olabilir
			return false;
		while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;
		if(transferKarti->deger - kartCaprazlama->deger == 1){// liste bo� de�ilse, son de�eri listele - kart de�eri 1 olmal�d�r
			kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
	}

	if(hangilistetipi == 3){//hangilistetipi 3 i�in tahtadan tahtaya
		if(kartCaprazlama == NULL && transferKarti->deger == 13){//liste bo�sa kart�n de�eri K = 13 olmal�d�r			
secilenListe->kartlar = transferKarti;
			return true;
		}
		if(!kartCaprazlama)//kartDonusu cannot be null here, can be null only for deger K=13
			return false;
		while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;

		if(!kartCaprazlama->dogrumu){//kart kapal� ise, iki kartlar aras�ndaki renk ve de�er �nemli de�ildir			
kartCaprazlama->next = transferKarti;
			return true;
		}
		if(kartCaprazlama->dogrumu)//kart tak�l�ysa, iki kartlar aras�ndaki renk ve de�er �nemlidir		
		if(!(transferKarti->color == kartCaprazlama->color))//iki biti�ik kartlar aras�ndaki renkler farkl�ysa
		if(kartCaprazlama->deger - transferKarti->deger == 1 ){//Son de�eri listele - transfer kartlar� de�eri - 1 olmal�d�r
			kartCaprazlama->next = transferKarti;
			return true;
			}
	}
	if(hangilistetipi == 4){// hangilistetipi 4 i�in tahtadan oyun sonuna ekleme yapar
		if(kartCaprazlama == NULL && transferKarti->deger == 1){//Liste bo�sa kart�n de�eri A = 1 olmal�d�r
			secilenListe->kartlar = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(!kartCaprazlama)//Kart Caprazlama burada bo� olamaz, sadece A = 1 de�eri i�in bo� olabilir
			return false;
while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;
		if(transferKarti->deger - kartCaprazlama->deger == 1){//Liste bo� de�ilse, son de�eri listele - kart de�eri 1 olmal�d�r
			kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
	}
	return false;
}

void List::goster_ToplisttenDizmeye(){//Kullan�mdan girdi istiyor
	char Sembol_of_colors, Sembol_of_suits, gecicideger[4];
	int Sembol_of_numbers;
	cout << endl << "Ust listten bir kart seciniz:";
	scanf("%c %c %s", &Sembol_of_colors, &Sembol_of_suits, gecicideger);

	if(gecicideger[0] == 'A')	Sembol_of_numbers = 1;
	else if(gecicideger[0] == 'J')Sembol_of_numbers = 11;
	else if(gecicideger[0] == 'Q')Sembol_of_numbers = 12;
	else if(gecicideger[0] == 'K')Sembol_of_numbers = 13;
	else 
		sscanf(gecicideger, "%d", &Sembol_of_numbers);

	ToplisttenDizmeye(Sembol_of_colors, Sembol_of_suits, Sembol_of_numbers);
}

void List::ToplisttenDizmeye(char s_color, char s_suit, int s_deger){
	List_Veriler *listeGecisi;
	Card_Veriler *tasinacak = NULL, *kartDonusu, *kartKuyrugu, *geciciust_baslik = ust_baslik;

	if(ust_baslik == NULL){
		cout << "Ust liste bostur. Bundan dolayi hareket yapamazsiniz." << endl;
		return;
	}
		
	kartDonusu = ust_baslik;
	kartKuyrugu = ust_baslik;

	while(kartDonusu){
		if(kartDonusu->color == s_color && kartDonusu->suit == s_suit  && kartDonusu->deger == s_deger){//kart�n verilerinin kullan�c� taraf�ndan girilen verilerle ayn� olup olmad���n� kontrol etme
			tasinacak = kartDonusu;
			break;
		}
		kartKuyrugu = kartDonusu;
		kartDonusu = kartDonusu->next;
	}//while sonu

	if(tasinacak == NULL){
		cout << "Girilen kart ust (top) listte yoktur!" << endl;
		return;
	}

	int number;
	listeGecisi = oyunsonu_baslik;
	if(tasinacak->suit == 'S') number = 0;
	if(tasinacak->suit == 'H') number = 1;
	if(tasinacak->suit == 'D') number = 2;
	if(tasinacak->suit == 'C') number = 3;

	for(int i = 0; i < number; i++)
		listeGecisi=listeGecisi->next;

	/*Tasinacak d���m�n listedeki di�er d���mlerle ba�lant�s�n�n kesilmesi*/
	if(kartDonusu == kartKuyrugu)//tasinacak node is first node
		geciciust_baslik = geciciust_baslik->next;
	else 
		kartKuyrugu->next = kartDonusu->next;

	if(listeyeKartEkle (listeGecisi, tasinacak, 2)){// hareket ba�ar�l� olursa, ust_baslik ikinci kart d���m�n� g�sterir
		ust_baslik = geciciust_baslik;
		cout << "Hareket Basarili!" << endl;
	}
	else{// ba�ar�l� de�il ise
		if(!(kartDonusu == kartKuyrugu))// ilk d���m de�ilse, kartKuyrugu ile tasinacak aras�ndaki ba�lant� kurtar�l�r
			kartKuyrugu->next = tasinacak;
		cout << "Hatali Hareket!" << endl;
	}
}

void List::goster_ToplisttenTahtaya(){// Kart�n �st listesini tahta listesine ta��mak i�in kullan�c�dan girdi istiyor
	char Sembol_of_colors, Sembol_of_suits, gecicideger[4];
	int Sembol_of_numbers, a;
	cout << "Ust Listeden bir kart seciniz:";
	scanf("%c %c %s", &Sembol_of_colors, &Sembol_of_suits, gecicideger);

	if(gecicideger[0] == 'A')	Sembol_of_numbers = 1;
	else if(gecicideger[0] == 'J')Sembol_of_numbers = 11;
	else if(gecicideger[0] == 'Q')Sembol_of_numbers = 12;
	else if(gecicideger[0] == 'K')Sembol_of_numbers = 13;
	else 
		sscanf(gecicideger, "%d", &Sembol_of_numbers);

	cout << "Hedef Tahta Listesinin numarasini secin:";
	scanf("%d", &a);
	ToplisttenTahtaya(a, Sembol_of_colors, Sembol_of_suits, Sembol_of_numbers);

}

void List::ToplisttenTahtaya(int listeIndexi, char s_color, char s_suit, int s_deger){
	List_Veriler *listeGecisi;
	Card_Veriler *tasinacak = NULL, *kartDonusu, *kartKuyrugu, *geciciust_baslik = ust_baslik;

	if(ust_baslik == NULL){
		cout << " Ust liste bostur. Bundan dolayi hareket yapamazsiniz." << endl;
		return;
	}
		
	kartDonusu = ust_baslik;
	kartKuyrugu = ust_baslik;

	while(kartDonusu){
		if(kartDonusu->color == s_color && kartDonusu->suit == s_suit  && kartDonusu->deger == s_deger){
			tasinacak = kartDonusu;
			break;
		}
		kartKuyrugu = kartDonusu;
		kartDonusu = kartDonusu->next;
	}//while sonu

	if(tasinacak == NULL){
		cout << "Girilen kart ust listede yoktur!" << endl;
		return;
	}

	listeGecisi = tahta_baslik;
	for(int i = 1; i < listeIndexi; i++)
		listeGecisi = listeGecisi->next;

	/*Tasinacak d���m�n listedeki di�er d���mlerle ba�lant�s�n�n kesilmesi*/
	if(kartDonusu == kartKuyrugu)//tasinacak d���m� ilk d���m
		geciciust_baslik = geciciust_baslik->next;
	else // tasinacak d���m� ilk d���m de�il
		kartKuyrugu->next = kartDonusu->next;

	if(listeyeKartEkle (listeGecisi, tasinacak, 1)){// hareket ba�ar�l� olursa, ust_baslik ikinci kart d���m�n� g�sterir
		ust_baslik = geciciust_baslik;
		cout << "Hareket Basarili!" << endl;
	}
	else{// ba�ar�l� de�ilse
		if(!(kartDonusu == kartKuyrugu))// ilk d���m de�ilse, kartKuyrugu ile tasinacak aras�ndaki ba�lant� kurtar�l�r
			kartKuyrugu->next = tasinacak;
		cout << "Hareket Hatali!" << endl;
	}
}

void List::goster_TahtadanTahtaya(){
	char Sembol_of_colors, Sembol_of_suits, gecicideger[4];
	int Sembol_of_numbers, source, destination;

	cout <<  "Harekete basliyacak olunan tahta listesini giriniz:";
	scanf("%d", &source);
	cout <<  "Hedef tahta listesini giriniz:";
	scanf("%d", &destination);

	cin.ignore(1000, '\n');
	cout <<  "Harekete basliyacak tahta listesinden hedef listesine yollanicak karti seciniz:";
	scanf("%c %c %s", &Sembol_of_colors, &Sembol_of_suits, gecicideger);

	if(gecicideger[0] == 'A')	Sembol_of_numbers = 1;
	else if(gecicideger[0] == 'J')Sembol_of_numbers = 11;
	else if(gecicideger[0] == 'Q')Sembol_of_numbers = 12;
	else if(gecicideger[0] == 'K')Sembol_of_numbers = 13;
	else 
		sscanf(gecicideger, "%d", &Sembol_of_numbers);
	TahtadanTahtaya(source, destination, Sembol_of_colors, Sembol_of_suits, Sembol_of_numbers);
}

void List::TahtadanTahtaya(int nereden, int nereye, char s_color, char s_suit, int s_deger){
	List_Veriler *kaynakListesiGecisi = tahta_baslik, *hedefListeGecisi = tahta_baslik;
	Card_Veriler *tasinacak = NULL, *kartDonusu, *kartKuyrugu, *gecici_basslik;

	for(int i = 1; i < nereden; i++)// liste nereden den gidiyor
		kaynakListesiGecisi = kaynakListesiGecisi->next;
	gecici_basslik = kaynakListesiGecisi->kartlar;
	kartDonusu = gecici_basslik;
	kartKuyrugu = gecici_basslik;
	

	while(kartDonusu){
		if(kartDonusu->dogrumu)
			if(kartDonusu->color == s_color && kartDonusu->suit == s_suit  && kartDonusu->deger == s_deger){
				tasinacak = kartDonusu;
				break;
			}
		kartKuyrugu = kartDonusu;
		kartDonusu = kartDonusu->next;
	}//while sonu

	if(tasinacak == NULL){
		cout << "Yanl�� Hareket!" << endl;
		return;
	}

	for(int i = 1; i < nereye; i++)// liste nereye den gider
		hedefListeGecisi = hedefListeGecisi->next;

	/*Tasinacak d���m�n listedeki di�er d���mlerle ba�lant�s�n�n kesilmesi*/
	if(kartDonusu == kartKuyrugu)// tasinacak d���m� ilk d���m
		gecici_basslik = NULL;
	else //tasinacak d���m� ilk d���m de�il
		kartKuyrugu->next = NULL;

	if(listeyeKartEkle (hedefListeGecisi, tasinacak, 3)){// hareket ba�ar�l� olursa, ust_baslik ikinci kart d���m�n� g�sterir
		kaynakListesiGecisi->kartlar = gecici_basslik;
		if(!kartKuyrugu->dogrumu)// kaynakta ta��nan kart�n arkas�ndaki kart DOWN ise, UP d�nd�r�n
			kartKuyrugu->dogrumu = true;
		cout << "Hareket Basarili!" << endl;
		return;
	}
	if(!(kartDonusu == kartKuyrugu)){//ilk d���m de�ilse, kartKuyrugu ve tasinacak aras�ndaki ba�lant� kurtar�l�r
		kartKuyrugu->next = tasinacak;
		cout << "Hatali Hareket!" << endl;
	}
	
}

void List::goster_TahtadanDizmeye(){
	int source;
	cout <<  "Kaynak tahta listesini seciniz:";
	scanf("%d", &source);
	cin.ignore(1000, '\n');
	TahtadanDizmeye(source);
}

void List::TahtadanDizmeye(int nereden){
	List_Veriler *kaynakListesiGecisi = tahta_baslik, *hedefListeGecisi = oyunsonu_baslik;
	Card_Veriler *tasinacak = NULL, *kartDonusu, *kartKuyrugu, *gecici_basslik;
	for(int i = 1; i < nereden; i++)//kullan�c�n�n istedi�i listeye gider
		kaynakListesiGecisi = kaynakListesiGecisi->next;
	gecici_basslik = kaynakListesiGecisi->kartlar;
	kartDonusu = gecici_basslik;
	kartKuyrugu = gecici_basslik;

	while(kartDonusu->next){//son karta git
		kartKuyrugu = kartDonusu;
		kartDonusu = kartDonusu->next;
	}//while sonu

	tasinacak = kartDonusu;//kart listesindeki son kart


	int number;
	if(tasinacak->suit == 'S') number = 0;
	if(tasinacak->suit == 'H') number = 1;
	if(tasinacak->suit == 'D') number = 2;
	if(tasinacak->suit == 'C') number = 3;

	for(int i = 0; i < number; i++)
		hedefListeGecisi=hedefListeGecisi->next;

	/*Tasinacak d���m�n listedeki di�er d���mlerle ba�lant�s�n�n kesilmesi*/
	if(kartDonusu == kartKuyrugu)//tasinacak d���m� ilk d���m
		gecici_basslik = NULL;
	else // tasinacak d���m� ilk d���m de�il
		kartKuyrugu->next = NULL;

	if(listeyeKartEkle (hedefListeGecisi, tasinacak, 4)){// hareket ba�ar�l� olursa, y�netim kurulu listesinin kart ba�� ikinci kart d���m�n� g�sterir
		kaynakListesiGecisi->kartlar = gecici_basslik;
		if(!kartKuyrugu->dogrumu)// kaynakta ta��nan kart�n arkas�ndaki kart DOWN ise, UP d�nd�r�K
			kartKuyrugu->dogrumu = true;
		cout << "Hareket Basarili!" << endl;
		return;
	}
	if(!(kartDonusu == kartKuyrugu)){// ta��ma ba�ar�l� de�ilse ve kart ilk d���m de�ilse, kartKuyrugu ve tasinacak aras�ndaki ba�lant� kurtar�l�r
		kartKuyrugu->next = tasinacak;
		cout << "Hatali Hareket!" << endl;
	}
}

void List::clear_screen(){//Sistemi temizler
	#ifdef _WIN32//��letim sistemi windows ise "cls" kullan�l�r
		std::system("cls");
	#else//di�er sistemler i�in "clear"" kullan�n
		std::system ("clear");
	#endif
}

void List::close(){//Ba�l� t�m listeleri ve ba�l� listeleri t�m d���mleri sil
	List_Veriler *listeGecisi;
	Card_Veriler *kartDonusu;

	/*En iyi liste d���mlerini silme*/
	kartDonusu = ust_baslik;
	while(ust_baslik){
		kartDonusu = ust_baslik;
		ust_baslik = ust_baslik->next;
		delete kartDonusu;
	}

	/*Pano listesi d���mlerini silme*/
	listeGecisi = tahta_baslik;
	while(tahta_baslik){
		kartDonusu = tahta_baslik->kartlar;
		while(tahta_baslik->kartlar){
			kartDonusu = tahta_baslik->kartlar;
			tahta_baslik->kartlar = tahta_baslik->kartlar->next;
			delete kartDonusu;
		}
		listeGecisi = tahta_baslik;
		tahta_baslik = tahta_baslik->next;
		delete listeGecisi;
	}

	/*Temel listesi d���mlerini silme*/
	listeGecisi = oyunsonu_baslik;
	while(oyunsonu_baslik){
		kartDonusu = oyunsonu_baslik->kartlar;
		while(oyunsonu_baslik->kartlar){
			kartDonusu = oyunsonu_baslik->kartlar;
			oyunsonu_baslik->kartlar = oyunsonu_baslik->kartlar->next;
			delete kartDonusu;
		}
		listeGecisi = oyunsonu_baslik;
		oyunsonu_baslik = oyunsonu_baslik->next;
		delete listeGecisi;
	}
	
	fclose(fptr);//Okuma txt dosyas�n� kapatma
}

