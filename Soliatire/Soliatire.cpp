#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <cstdlib>

struct Card_Veriler{
	char color;
	char suit;
	int deger; // Deðerler = 1 için A, 11 için J, 12 için Q, 13 için K
	bool dogrumu; // Acik için doðru, Kapali için yanlýþ
	Card_Veriler *next;
};

struct List_Veriler{
	int listno;
	List_Veriler *next;
	Card_Veriler *kartlar;
};

struct List{
	Card_Veriler *ust_baslik;//Üst liste baþlýðý
	List_Veriler *oyunsonu_baslik;
	List_Veriler *tahta_baslik;
	FILE *fptr;
	
	void create();// Toplist, tahtaListeleri ve oyunSonuListeleri oluþturmayý içerir
	void create_toplist();
	void create_tahtaListeleri();
	void create_oyunSonuListeleri();
	void printkart(Card_Veriler *kart);
	void printlisteler();// Bütün listeleri yazdýrýr
	void printmenu();//Seçim menüsü
	bool listeyeKartEkle(List_Veriler *secilenListe, Card_Veriler *transferKarti, int hangilistetipi);//Aktarýlan kartý seçilen listeye ekler, hangi liste türü program tarafýndan girilir
	void goster_ToplisttenDizmeye();//Girdi alýr ve ilgili iþlev çaðrýlarý
	void ToplisttenDizmeye(char s_color, char s_suit, int s_deger);
	void goster_ToplisttenTahtaya();
	void ToplisttenTahtaya(int listeIndexi, char s_color, char s_suit, int s_deger);
	void goster_TahtadanTahtaya();
	void TahtadanTahtaya(int nereden, int nereye, char s_color, char s_suit, int s_deger);//kartlarý tahta üzerinde hareket ettirir
	void goster_TahtadanDizmeye();//girdilerle ve çaðrýlarla ilgili iþlev
	void TahtadanDizmeye(int nereden);//tahtadaki kartý oyun sonu dizimine ekler
	void islem_yap();//yapýlacak iþlem sevilir
	bool ToplistBosmu, tahtaBosmu;// oyun sonu kontrol için gerekli
	void clear_screen();//ekran temizleme fonksiyonu
	void close();//kapatma fonksiyonu
};

List islem;
using namespace std;

int main()
{
	char secim;
	islem.create();
	if(!islem.fptr)//Solitaire.txt dosyasýný bulamazsanýz, programý kapatýn
	return 0;

	islem.ToplistBosmu= false;//Üst liste boþsa, bu deðer doðru olacaktýr
	islem.tahtaBosmu = false;//Pano listesi boþsa, bu deðer doðru olacaktýr

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

		if(!islem.ust_baslik)// üst listeyi kontrol et veya boþ býrak
			islem.ToplistBosmu= true;	
		List_Veriler *capraz;
		capraz = islem.tahta_baslik;
		int sayac = 0;
		while(capraz){//pano listelerini kontrol etmek boþ ya da deðil
			if(!capraz->kartlar)
				sayac++;
			capraz = capraz->next;
			if(sayac == 7)//7 tahta listesinin tümü boþsa sayaç 7'dir
				islem.tahtaBosmu = true;
		}

		/*Oyun tamamlandýðýnda kullanýcýya mesaj yazdýrýr*/
		if(islem.tahtaBosmu && islem.ToplistBosmu){//Pano ve üst listeler boþken döngü sýrasýnda dur
			printf("\n\n\n\t\t\tOYUNU KAZANDINIZ!!!! OYUN BITTI! TEBRIKLER!\n");// oyun bitti mesajý
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
		cout << "'solitaire.txt' dosyasý bulunamadý.Program kapatýlacak."<< endl;
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
		if(gecicicolor == '*'){//ilk yýldýz okuma
				fscanf(fptr, "%6c", garbage);// diðer beþ yýldýzý geçer ve yeni çizgiye gider 
				break;
		}
		else
			newnode->color = gecicicolor;

		fscanf(fptr, " %c %s %s ", &gecicisuit, gecicideger, gecicidogrumu);
		newnode->suit = gecicisuit;
		
		/*Char karakterler int deðerlere aktarýlýyor*/
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
		if(ust_baslik == NULL){//ilk düðümü boþ top liste ekleme
			ust_baslik = newnode;
			final = ust_baslik;
		}
		else{//düðümü listeye ekleme
			final->next = newnode;
			final = final->next;
		}
	}//while sonu
}

void List::create_tahtaListeleri(){
	Card_Veriler *newnode, *final;//kart listesinin son düðümüne son noktalar
	List_Veriler *yeniTahtaListesi, *tahtaListesiFinal;// tahtaListesiFinal puanlarýnýn son düðümüne son noktalar
	char gecicicolor, gecicisuit, gecicideger[4], gecicidogrumu[8], garbage[10];
	int index = 1;// Bu dizin pano listesini temsil eder
	yeniTahtaListesi = new List_Veriler;// ilk tahta listesini yaratýr
	tahta_baslik = yeniTahtaListesi;
	tahtaListesiFinal = yeniTahtaListesi;
	yeniTahtaListesi->listno = index++;
	yeniTahtaListesi->next = NULL;
	yeniTahtaListesi->kartlar = NULL;

	while(!feof(fptr)){//bu while döngüsü tahta multi linked listelerini oluþturur		
	newnode = new Card_Veriler;
		newnode->next = NULL;
		fscanf(fptr, "%c", &gecicicolor);
		if(gecicicolor == '*'){//ilk yýldýz okuma
				fscanf(fptr, "%6c", garbage);//diðer beþ yýldýzý geçer ve yeni çizgiye gider
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

		if(tahtaListesiFinal->kartlar == NULL){//ilk düðümü boþ tahta listesine ekle
			tahtaListesiFinal->kartlar = newnode;
			final = newnode;
		}
		else{//boþ tahta listesine ekle
			final->next = newnode;
			final = final->next;
		}
	}//while sonu
}

void List::create_oyunSonuListeleri(){
	oyunsonu_baslik = NULL;
	List_Veriler *yeniListe, *finalListe;
	int index = 1;//Maça liste indexi = 1, Kupa liste indexi = 2, Karo liste indexi = 3, Sinek liste indexi = 4

	/*Maça Listesini baþlatma*/
	yeniListe = new List_Veriler;
	yeniListe->kartlar = NULL;
	yeniListe->next = NULL;
	yeniListe->listno = index++;
	oyunsonu_baslik = yeniListe;
	finalListe = yeniListe;

	/*Diðer 3 listeyi baþlatma*/
	for(int i = 0 ; i <3; i++)	{
		yeniListe = new List_Veriler;
		yeniListe->kartlar = NULL;
		yeniListe->next = NULL;
		yeniListe->listno = index++;
		finalListe->next = yeniListe;
		finalListe = yeniListe;
	}//for sonu
}// fonksiyon sonu

void List::printkart(Card_Veriler *kart){// kart düðümünün verilerini yazdýrýr
	if(!kart->dogrumu ){//Kapalý ise kartý gizle
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
	Card_Veriler *ct[7];//Kart listesi kart düðümü  ; ct[0] 1.liste için, ct[1] 2.liste için ....
	Card_Veriler *foundct[4];//Bulunan Liste Kartý Düðüm, foundct[0] = Maçalar, foundct[1] = Kupalar, foundct[2] = Karolar, foundct[3] = Sinekler 
	Card_Veriler *topct;// TopList Kard tarayýcý
	List_Veriler *listeGecisi;// Liste Düðümü Gezgini


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
	for(int i = 0; i < 7; i++){// ct[] pointerlerinin baþlatýlmasý
		ct[i] = listeGecisi->kartlar;
		listeGecisi = listeGecisi->next;
	}

	/*Tahta listelerindeki kartlarýn yazdýrýlmazý*/
	for(int i = 0; i < 19; i++){//döngü için listeler arasýnda geçiþ yapar ve 19 kez aþaðý iner, bir liste 19'a kadar kart içerebilir (6 aþaðý kart + 13 yukarý kart) 
		for(int j = 0; j < 7; j++)
		{
			if(ct[j]){//ct [j] boþ deðilse, yazdýrýn ve sonraki düðüme gidin
				printkart(ct[j]);
				printf("\t");
				ct[j] = ct[j]->next;
			}
			else//ct [j] boþsa, bir sekme yazdýrýn
				printf("\t");
		}
		if(ct[0] || ct[1] || ct[2] || ct[3] || ct[4] || ct[5] || ct[6])// bir hat yazdýrdýktan sonra; 
			printf("\n");// en az bir kart boþ deðilse: yeni satýra git
			else
			break;// satýrdaki tüm kartlar null ise: loop için dýþtan ayýr
	}//dýþ için

	cout << endl << "Oyun sonu Listeleri:" << endl;
	cout << "Maca\tKupa\tKaro\tSinek" << endl;

	listeGecisi = oyunsonu_baslik;
	for(int i = 0; i < 4; i++){//foundct[] pointerlerinin baþlatýlmasý
		foundct[i] = listeGecisi->kartlar;
		listeGecisi = listeGecisi->next;
	}

	for(int i = 0; i < 13; i++){//döngü için temel listelerini geçer (en fazla 13 kart bir temel listesinde olabilir)
		for(int j = 0; j < 4; j++)
		{
			if(foundct[j]){//ct [j] boþ deðilse, yazdýrýn ve sonraki düðüme gidin
				printkart(foundct[j]);
				printf("\t");
				foundct[j] = foundct[j]->next;
			}
			else//foundct [j] boþsa, bir sekme yazdýrýn
				printf("\t");
		}
		if(foundct[0] || foundct[1] || foundct[2] || foundct[3])//bir line yazdýktan sonra; 
			printf("\n");//en az bir kart boþ deðilse: yeni satýra git
		else
			break;//satýrdaki tüm kartlar null ise: loop için dýþtan ayýr
	}//dýþ için
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
bool List::listeyeKartEkle(List_Veriler *secilenListe, Card_Veriler *transferKarti, int hangilistetipi){// hangi liste türü kullanýcý ile ilgili deðil, program tarafýndan otomatik olarak girilir
	Card_Veriler *kartCaprazlama;
	kartCaprazlama= secilenListe->kartlar;
	if(hangilistetipi == 1){//hangilistetipi 1 olmasý için tahtada
		if(kartCaprazlama == NULL && transferKarti->deger == 13){//liste boþsa kartýn deðeri K = 13 olmalýdýr
			secilenListe->kartlar = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(!kartCaprazlama)//kart geçiþi burada boþ olamaz, sadece K = 13 deðeri için boþ olabilir
			return false;
		while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;

		if(!kartCaprazlama->dogrumu){//kart kapalý ise, iki kart arasýndaki renk ve deðer önemli deðildir			
kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(kartCaprazlama->dogrumu)// kart takýlýysa, iki kart arasýndaki renk ve deðer önemlidir		
		if(!(transferKarti->color == kartCaprazlama->color))// iki bitiþik kart arasýndaki renkler farklýysa				
if(kartCaprazlama->deger - transferKarti->deger == 1 ){// Son deðeri listele - transfer kartlarý deðeri - 1 olmalýdýr
			kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
	}

	if(hangilistetipi == 2){// hangilistetipi 2 dir çünkü top listten oyun sonuna taþýmak için.
		if(kartCaprazlama == NULL && transferKarti->deger == 1){// Liste boþsa kartýn deðeri A = 1 olmalýdýr
			secilenListe->kartlar = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(!kartCaprazlama)// kart geçidi burada boþ olamaz, sadece A = 1 deðeri için boþ olabilir
			return false;
		while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;
		if(transferKarti->deger - kartCaprazlama->deger == 1){// liste boþ deðilse, son deðeri listele - kart deðeri 1 olmalýdýr
			kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
	}

	if(hangilistetipi == 3){//hangilistetipi 3 için tahtadan tahtaya
		if(kartCaprazlama == NULL && transferKarti->deger == 13){//liste boþsa kartýn deðeri K = 13 olmalýdýr			
secilenListe->kartlar = transferKarti;
			return true;
		}
		if(!kartCaprazlama)//kartDonusu cannot be null here, can be null only for deger K=13
			return false;
		while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;

		if(!kartCaprazlama->dogrumu){//kart kapalý ise, iki kartlar arasýndaki renk ve deðer önemli deðildir			
kartCaprazlama->next = transferKarti;
			return true;
		}
		if(kartCaprazlama->dogrumu)//kart takýlýysa, iki kartlar arasýndaki renk ve deðer önemlidir		
		if(!(transferKarti->color == kartCaprazlama->color))//iki bitiþik kartlar arasýndaki renkler farklýysa
		if(kartCaprazlama->deger - transferKarti->deger == 1 ){//Son deðeri listele - transfer kartlarý deðeri - 1 olmalýdýr
			kartCaprazlama->next = transferKarti;
			return true;
			}
	}
	if(hangilistetipi == 4){// hangilistetipi 4 için tahtadan oyun sonuna ekleme yapar
		if(kartCaprazlama == NULL && transferKarti->deger == 1){//Liste boþsa kartýn deðeri A = 1 olmalýdýr
			secilenListe->kartlar = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
		if(!kartCaprazlama)//Kart Caprazlama burada boþ olamaz, sadece A = 1 deðeri için boþ olabilir
			return false;
while(kartCaprazlama->next)
			kartCaprazlama = kartCaprazlama->next;
		if(transferKarti->deger - kartCaprazlama->deger == 1){//Liste boþ deðilse, son deðeri listele - kart deðeri 1 olmalýdýr
			kartCaprazlama->next = transferKarti;
			transferKarti->next = NULL;
			return true;
		}
	}
	return false;
}

void List::goster_ToplisttenDizmeye(){//Kullanýmdan girdi istiyor
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
		if(kartDonusu->color == s_color && kartDonusu->suit == s_suit  && kartDonusu->deger == s_deger){//kartýn verilerinin kullanýcý tarafýndan girilen verilerle ayný olup olmadýðýný kontrol etme
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

	/*Tasinacak düðümün listedeki diðer düðümlerle baðlantýsýnýn kesilmesi*/
	if(kartDonusu == kartKuyrugu)//tasinacak node is first node
		geciciust_baslik = geciciust_baslik->next;
	else 
		kartKuyrugu->next = kartDonusu->next;

	if(listeyeKartEkle (listeGecisi, tasinacak, 2)){// hareket baþarýlý olursa, ust_baslik ikinci kart düðümünü gösterir
		ust_baslik = geciciust_baslik;
		cout << "Hareket Basarili!" << endl;
	}
	else{// baþarýlý deðil ise
		if(!(kartDonusu == kartKuyrugu))// ilk düðüm deðilse, kartKuyrugu ile tasinacak arasýndaki baðlantý kurtarýlýr
			kartKuyrugu->next = tasinacak;
		cout << "Hatali Hareket!" << endl;
	}
}

void List::goster_ToplisttenTahtaya(){// Kartýn üst listesini tahta listesine taþýmak için kullanýcýdan girdi istiyor
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

	/*Tasinacak düðümün listedeki diðer düðümlerle baðlantýsýnýn kesilmesi*/
	if(kartDonusu == kartKuyrugu)//tasinacak düðümü ilk düðüm
		geciciust_baslik = geciciust_baslik->next;
	else // tasinacak düðümü ilk düðüm deðil
		kartKuyrugu->next = kartDonusu->next;

	if(listeyeKartEkle (listeGecisi, tasinacak, 1)){// hareket baþarýlý olursa, ust_baslik ikinci kart düðümünü gösterir
		ust_baslik = geciciust_baslik;
		cout << "Hareket Basarili!" << endl;
	}
	else{// baþarýlý deðilse
		if(!(kartDonusu == kartKuyrugu))// ilk düðüm deðilse, kartKuyrugu ile tasinacak arasýndaki baðlantý kurtarýlýr
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
		cout << "Yanlýþ Hareket!" << endl;
		return;
	}

	for(int i = 1; i < nereye; i++)// liste nereye den gider
		hedefListeGecisi = hedefListeGecisi->next;

	/*Tasinacak düðümün listedeki diðer düðümlerle baðlantýsýnýn kesilmesi*/
	if(kartDonusu == kartKuyrugu)// tasinacak düðümü ilk düðüm
		gecici_basslik = NULL;
	else //tasinacak düðümü ilk düðüm deðil
		kartKuyrugu->next = NULL;

	if(listeyeKartEkle (hedefListeGecisi, tasinacak, 3)){// hareket baþarýlý olursa, ust_baslik ikinci kart düðümünü gösterir
		kaynakListesiGecisi->kartlar = gecici_basslik;
		if(!kartKuyrugu->dogrumu)// kaynakta taþýnan kartýn arkasýndaki kart DOWN ise, UP döndürün
			kartKuyrugu->dogrumu = true;
		cout << "Hareket Basarili!" << endl;
		return;
	}
	if(!(kartDonusu == kartKuyrugu)){//ilk düðüm deðilse, kartKuyrugu ve tasinacak arasýndaki baðlantý kurtarýlýr
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
	for(int i = 1; i < nereden; i++)//kullanýcýnýn istediði listeye gider
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

	/*Tasinacak düðümün listedeki diðer düðümlerle baðlantýsýnýn kesilmesi*/
	if(kartDonusu == kartKuyrugu)//tasinacak düðümü ilk düðüm
		gecici_basslik = NULL;
	else // tasinacak düðümü ilk düðüm deðil
		kartKuyrugu->next = NULL;

	if(listeyeKartEkle (hedefListeGecisi, tasinacak, 4)){// hareket baþarýlý olursa, yönetim kurulu listesinin kart baþý ikinci kart düðümünü gösterir
		kaynakListesiGecisi->kartlar = gecici_basslik;
		if(!kartKuyrugu->dogrumu)// kaynakta taþýnan kartýn arkasýndaki kart DOWN ise, UP döndürüK
			kartKuyrugu->dogrumu = true;
		cout << "Hareket Basarili!" << endl;
		return;
	}
	if(!(kartDonusu == kartKuyrugu)){// taþýma baþarýlý deðilse ve kart ilk düðüm deðilse, kartKuyrugu ve tasinacak arasýndaki baðlantý kurtarýlýr
		kartKuyrugu->next = tasinacak;
		cout << "Hatali Hareket!" << endl;
	}
}

void List::clear_screen(){//Sistemi temizler
	#ifdef _WIN32//Ýþletim sistemi windows ise "cls" kullanýlýr
		std::system("cls");
	#else//diðer sistemler için "clear"" kullanýn
		std::system ("clear");
	#endif
}

void List::close(){//Baðlý tüm listeleri ve baðlý listeleri tüm düðümleri sil
	List_Veriler *listeGecisi;
	Card_Veriler *kartDonusu;

	/*En iyi liste düðümlerini silme*/
	kartDonusu = ust_baslik;
	while(ust_baslik){
		kartDonusu = ust_baslik;
		ust_baslik = ust_baslik->next;
		delete kartDonusu;
	}

	/*Pano listesi düðümlerini silme*/
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

	/*Temel listesi düðümlerini silme*/
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
	
	fclose(fptr);//Okuma txt dosyasýný kapatma
}

