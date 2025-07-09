create table USERS (
	user_id INTEGER PRIMARY KEY,
        tutorial boolean DEFAULT false,
	name varchar(50)
);

create table LESSON_CATEGORIES (
	category_id INTEGER PRIMARY KEY,
	name varchar(200)
);

create table LESSON_QUESTIONS (
	question_id INTEGER PRIMARY KEY,
	lesson_id int,
	question_pos int,
	question_type int,
	question varchar(200),
	choice1 varchar(50),
	choice2 varchar(50),
	choice3 varchar(50),
	choice4 varchar(50),
	answer int NOT NULL,
	image varchar(50) ,
	FOREIGN KEY (lesson_id) REFERENCES LESSONS(lesson_id)
);

create table LESSONS (
	lesson_id INTEGER PRIMARY KEY,
	category_id int NOT NULL DEFAULT 1,
	title varchar(200),
	page_name varchar(50),
	question_count int,
	FOREIGN KEY (category_id) REFERENCES LESSON_CATEGORIES(category_id)
);

create table LESSON_RESULTS (
	result_id INTEGER PRIMARY KEY,
	correct_count int,
	user_id int,
	lesson_id int,
	UNIQUE(user_id, lesson_id),
	FOREIGN KEY (user_id) REFERENCES USERS(user_id),
	FOREIGN KEY (lesson_id) REFERENCES LESSONS(lesson_id)
);

create table TUTORIAL (
	id INTEGER PRIMARY KEY,
	text varchar(200),
	image varchar(200)
);

insert into TUTORIAL values 
    (NULL,'Pateaya Hoşgeldin!' || char(13) || char(10) || 'Burada pardus ve bilgisayar dünyası hakkında yeni bilgiler edineceksin!','/org/gtk/patea/image/logo'),
    (NULL,'Pateaya giriş yaptığında karşına farklı farklı bilgiler edinebileceğin birbirinden güzel testler çıkacak.','/org/gtk/patea/image/brain'),
    (NULL,'Testlerde 2 tip soru var:' || char(13) || char(10) || '1. Çoktan seçmeli 4.şıklı sorular,' || char(13) || char(10) || '2. Terminalde komutlar çalıştıracağın uygulamalı sorular.','/org/gtk/patea/image/crossroad'),
    (NULL,'Başarılar!!!','/org/gtk/patea/image/logo');

insert into USERS (user_id, name) values 
    (NULL, 'Kullanıcı 1'),
    (NULL, 'Kullanıcı 2');

insert into LESSON_CATEGORIES values 
    (NULL, 'Genel'),
    (NULL, 'Bilgisayarlar'),
    (NULL, 'Linux'),
    (NULL, 'Pardus'),
    (NULL, 'Terminal'),
    (NULL, 'Tarih');

insert into LESSONS values (NULL, 1, 'Genel Kültür Testi', 'genel_kultur_1', 7);
insert into LESSON_QUESTIONS values 
    (NULL, 1, 0, 0, 'Bir metin belgesindeki yazıyı kopyalamak için hangi klavye kısayolu kullanılır?',
        'A: Ctrl + X', 'B: Ctrl + C', 'C: Ctrl + V', 'D: Ctrl + A', 1, '/org/gtk/patea/image/kes_kopyala_yapistir'),
    (NULL, 1, 1, 0, 'Bir metin belgesindeki yazıyı kesmek için hangi klavye kısayolu kullanılır?',
        'A: Ctrl + X', 'B: Ctrl + C', 'C: Ctrl + V', 'D: Ctrl + A', 0, '/org/gtk/patea/image/kes_kopyala_yapistir'),
    (NULL, 1, 2, 0, 'Bir metin belgesindeki yazıyı yapıştırmak için hangi klavye kısayolu kullanılır?',
        'A: Ctrl + X', 'B: Ctrl + C', 'C: Ctrl + V', 'D: Ctrl + A', 2, '/org/gtk/patea/image/kes_kopyala_yapistir'),
    (NULL, 1, 3, 0, 'Bir metin belgesindeki tüm yazıları seçmek için hangi klavye kısayolu kullanılır?',
        'A: Ctrl + X', 'B: Ctrl + C', 'C: Ctrl + V', 'D: Ctrl + A', 3, '/org/gtk/patea/image/kes_kopyala_yapistir'),
    (NULL, 1, 4, 0, 'Aşağıdakilerden hangisi bir dosya uzantısıdır?',
        'A: .com', 'B: .txt', 'C: .net', 'D: .org', 1, ''),
    (NULL, 1, 5, 0, 'İnternette arama yapmak için kullanılan web sitelerine ne ad verilir?',
        'A: Sosyal Medya', 'B: E-posta Servisi', 'C: Arama Motoru', 'D: Çevrimiçi Mağaza', 2, ''),
    (NULL, 1, 6, 0, 'Bir e-posta göndermek için alıcının neyini bilmeniz gerekir?',
        'A: Telefon Numarası', 'B: Ev Adresi', 'C: IP Adresi', 'D: E-posta Adresi', 3, '');

insert into LESSONS values (NULL, 1, 'Temel Kavramlar Testi', 'temel_kavramlar_1', 3);
insert into LESSON_QUESTIONS values 
    (NULL, 2, 0, 0, 'Bir bilgisayarı veya programı yeniden başlatma işlemine ne denir?',
        'A: Kapatma (Shutdown)', 'B: Yeniden Başlatma (Restart/Reboot)', 'C: Yükleme (Install)', 'D: Silme (Delete)', 1, ''),
    (NULL, 2, 1, 0, 'İçerisinde dosyaların ve başka klasörlerin saklandığı dijital birime ne ad verilir?',
        'A: Klasör (Dizin)', 'B: Kısayol', 'C: Simge', 'D: Pencere', 0, ''),
    (NULL, 2, 2, 0, 'İnternet ortamında kimlik hırsızlığı yapmak anlamına gelen terim hangisidir?',
        'A: Spam', 'B: Phishing (Oltalama)', 'C: Firewall (Güvenlik Duvarı)', 'D: Cookie (Çerez)', 1, '');

insert into LESSONS values (NULL, 2, 'Donanım Bileşenleri', 'bilgisayar_donanim_1', 5);
insert into LESSON_QUESTIONS values 
    (NULL, 3, 0, 0, 'CPU'' nun temel görevi nedir?',
        'A: Görüntü işlemek', 'B: Verileri kalıcı depolamak', 'C: Aritmetik ve mantıksal işlemleri yapmak', 'D: İnternete bağlanmak', 2, ''),
    (NULL, 3, 1, 0, 'SSD ve HDD arasındaki temel fark nedir?',
        'A: SSD daha yavaş çalışır.', 'B: SSD hareketli parçalara sahip değildir.', 'C: HDD daha az enerji harcar.', 'D: SSD sadece laptoplarda kullanılır.', 1, ''),
    (NULL, 3, 2, 0, 'Tüm donanım birimlerinin üzerine takıldığı ana devre kartı hangisidir?',
        'A: Ekran Kartı', 'B: Anakart', 'C: Ses Kartı', 'D: Güç Kaynağı', 1, ''),
    (NULL, 3, 3, 0, 'Bilgisayar çalışırken programların geçici olarak depolandığı bellek birimi hangisidir?',
        'A: ROM', 'B: Sabit Disk', 'C: RAM', 'D: Flash Bellek', 2, ''),
    (NULL, 3, 4, 0, 'Aşağıdakilerden hangisi bilgisayar çalışması için zorunludur ?',
        'A: Ekran', 'B: Klavye', 'C: RAM', 'D: Fare', 2, '');

insert into LESSONS values (NULL, 2, 'Yazılım ve İşletim Sistemleri', 'bilgisayar_yazilim_1', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 4, 0, 0, 'Aşağıdakilerden hangisi bir işletim sistemidir?',
        'A: Microsoft Word', 'B: Adobe Photoshop', 'C: Google Chrome', 'D: Linux', 3, ''),
    (NULL, 4, 1, 0, 'Kaynak kodları herkese açık olan yazılımlara ne ad verilir?',
        'A: Ücretli Yazılım', 'B: Açık Kaynak Kodlu Yazılım', 'C: Kapalı Kaynak Kodlu Yazılım', 'D: Shareware', 1, ''),
    (NULL, 4, 2, 0, 'Bir programdaki hataları bulma ve düzeltme işlemine ne denir?',
        'A: Derleme (Compilation)', 'B: Yorumlama (Interpretation)', 'C: Hata Ayıklama (Debugging)', 'D: Kurulum (Installation)', 2, ''),
    (NULL, 4, 3, 0, 'Bilgisayarın donanım kaynaklarını yöneten temel yazılım hangisidir?',
        'A: Uygulama Yazılımı', 'B: Sürücü (Driver)', 'C: İşletim Sistemi (OS)', 'D: BIOS', 2, '');

insert into LESSONS values (NULL, 3, 'Linux''a Giriş', 'linux_giris_1', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 5, 0, 0, 'Linux çekirdeğini (kernel) ilk geliştiren kişi kimdir?',
        'A: Richard Stallman', 'B: Bill Gates', 'C: Steve Jobs', 'D: Linus Torvalds', 3, '/org/gtk/patea/image/linux'),
    (NULL, 5, 1, 0, 'Linux işletim sisteminde en yetkili kullanıcıya ne ad verilir?',
        'A: admin', 'B: guest', 'C: root', 'D: user', 2, ''),
    (NULL, 5, 2, 0, 'Linux çekirdeği üzerine kurulu, çeşitli yazılımlar eklenerek oluşturulan tam işletim sistemlerine ne denir?',
        'A: Sürüm', 'B: Dağıtım (Distribution)', 'C: Çekirdek', 'D: Paket', 1, ''),
    (NULL, 5, 3, 0, 'Aşağıdakilerden hangisi bir Linux dağıtımı değildir?',
        'A: Pardus', 'B: Fedora', 'C: Windows 11', 'D: Debian', 2, '');

insert into LESSONS values (NULL, 3, 'Linux Dosya Sistemi', 'linux_dosya_sistemi_1', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 6, 0, 0, 'Linux dosya sisteminde en üst dizin (kök dizin) hangisidir?',
        'A: /home', 'B: /root', 'C: /', 'D: C:\\', 2, ''),
    (NULL, 6, 1, 0, 'Bir dosyanın izinlerini değiştirmek için hangi terminal komutu kullanılır?',
        'A: chown', 'B: chmod', 'C: chgrp', 'D: chdir', 1, ''),
    (NULL, 6, 2, 1, 'Kullanıcı adınızı terminal ekranına yazdırın. (echo kullanmayın!)',
        'whoami', '', '', '', 2, ''),
    (NULL, 6, 3, 1, 'Ev dizininizde (home) "deneme" adında bir dosya oluşturun.',
        'test -f ~/deneme', '', '', '', 1, '');

insert into LESSONS values (NULL, 4, 'Pardus', 'pardus_ozel_1', 3);
insert into LESSON_QUESTIONS values 
    (NULL, 7, 0, 0, 'Pardus projesi hangi ulusal kurum tarafından desteklenmektedir?',
        'A: ASELSAN', 'B: TUSAŞ', 'C: TÜBİTAK ULAKBİM', 'D: ROKETSAN', 2, '/org/gtk/patea/image/pardus'),
    (NULL, 7, 1, 0, 'Pardus, hangi Linux dağıtımının kararlı sürümünü temel alır?',
        'A: Debian', 'B: Arch Linux', 'C: CentOS', 'D: Gentoo', 0, '/org/gtk/patea/image/pardus'),
    (NULL, 7, 2, 0, 'Pardus''un güncel sürümlerinde hangi masaüstü ortamları sunulur?',
        'A: GNOME ve Cinnamon', 'B: KDE Plasma ve MATE', 'C: XFCE ve GNOME', 'D: MATE ve Deepin', 2, '/org/gtk/patea/image/pardus');

insert into LESSONS values (NULL, 5, 'Terminal Giriş', 'terminal_temel_0', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 8, 0, 0, 'Linux terminalinde bulunduğun klasörü görmek için hangi komut kullanılır?',
        'A: ls', 'B: pwd', 'C: cd', 'D: echo', 1, ''),
    (NULL, 8, 1, 0, 'Aşağıdaki komutlardan hangisi mevcut dizindeki dosyaları listeler?',
        'A: cd', 'B: mkdir', 'C: ls', 'D: touch', 2, ''),
    (NULL, 8, 2, 0, 'Yeni bir klasör oluşturmak için hangi komut kullanılır?',
        'A: mkdir', 'B: rmdir', 'C: rm', 'D: cd', 0, ''),
    (NULL, 8, 3, 0, 'Bir dosyayı silmek için hangi komut kullanılır?',
        'A: mv', 'B: touch', 'C: rm', 'D: ls', 2, '');

insert into LESSONS values (NULL, 5, 'Temel Terminal Komutları', 'terminal_temel_1', 10);
insert into LESSON_QUESTIONS values 
    (NULL, 9, 0, 0, 'Bulunulan dizini göstermek için hangi komut kullanılır?',
        'A: whereami', 'B: pwd', 'C: ls', 'D: cd', 1, ''),
    (NULL, 9, 1, 1, 'Bulunulan dizini yazdırın.'|| char(13) || char(10),
        'pwd', '', '', '', 2, ''),
    (NULL, 9, 2, 0, 'Yeni bir klasör oluşturmak için kullanılan komut hangisidir?',
        'A: touch', 'B: make', 'C: dir', 'D: mkdir', 3, ''),
    (NULL, 9, 3, 1, '"patea" adında bir klasör oluşturun.'|| char(13) || char(10),
        'mkdir patea', '', '', '', 2, ''),
    (NULL, 9, 4, 0, 'Bir klasöre girmek için kullanılan komut hangisidir?',
        'A: touch', 'B: source', 'C: echo', 'D: cd', 3, ''),
    (NULL, 9, 5, 1, 'Oluşturduğunuz "patea" isimli klasöre girin.'|| char(13) || char(10), 
        'cd patea', '', '', '', 2, ''),
    (NULL, 9, 6, 0, 'Bulunduğumuz klasörden çıkmak için kullanılan komut hangisidir?',
        'A: cd ..', 'B: cd ~', 'C: cd /', 'D: cd -', 0, ''),
    (NULL, 9, 7, 1, 'Oluşturduğunuz "patea" isimli klasörden çıkın.'|| char(13) || char(10),
        'cd ..', '', '', '', 2, ''),
    (NULL, 9, 8, 0, 'Boş bir klasörü silmek için kullanılan komut hangisidir?',
        'A: cd remove', 'B: rmdir', 'C: kill', 'D: del', 1, ''),
    (NULL, 9, 9, 1, 'Oluşturduğunuz "patea" isimli klasörü silin.'|| char(13) || char(10),  
        'rmdir patea', '', '', '', 2, '');

insert into LESSONS values (NULL, 5, 'İleri Seviye Terminal', 'terminal_ileri_1', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 10, 0, 0, 'Bir dosyanın içeriğini ekrana yazdırmak için hangi komut kullanılır?',
        'A: echo', 'B: print', 'C: cat', 'D: type', 2, ''),
    (NULL, 10, 1, 0, 'Bir komutun çıktısını bir dosyaya yazdırmak için hangi operatör kullanılır?',
        'A: | (pipe)', 'B: & (ampersand)', 'C: $ (dolar)', 'D: > (büyüktür)', 3, ''),
    (NULL, 10, 2, 0, 'Sisteminizdeki bellek (RAM) miktarını yazdırmak için hangi komut kullanılır?',
        'A: free -h', 'B: ram', 'C: info ram', 'D: lsram', 0, ''),
    (NULL, 10, 3, 1, '''free -h'' komutunu çalıştırarak çıktısını ''ram.txt'' adlı dosyaya yazdırın.', 
        'free -h > ram.txt', '', '', '', 2, '');

insert into LESSONS values (NULL, 6, 'Bilgisayar Tarihi', 'tarih_bilgisayar_1', 5);
insert into LESSON_QUESTIONS values 
    (NULL, 11, 0, 0, 'Modern bilgisayarın babası olarak kabul edilen İngiliz matematikçi kimdir?',
        'A: Alan Turing', 'B: Charles Babbage', 'C: John von Neumann', 'D: Blaise Pascal', 1, ''),
    (NULL, 11, 1, 0, 'İlk genel amaçlı, elektronik bilgisayar olarak kabul edilen makine hangisidir?',
        'A: Colossus', 'B: Z3', 'C: ENIAC', 'D: Mark I', 2, ''),
    (NULL, 11, 2, 0, 'İlk ticari olarak başarılı mikroişlemci olan Intel 4004 kaç yılında piyasaya sürülmüştür?',
        'A: 1965', 'B: 1971', 'C: 1980', 'D: 1991', 1, ''),
    (NULL, 11, 3, 0, 'İlk kişisel bilgisayarlardan biri olan Apple I'' i kim tasarlamıştır?', 
        'A: Bill Gates', 'B: Steve Wozniak', 'C: Paul Allen', 'D: Steve Jobs', 1, ''),
    (NULL, 11, 4, 0, 'Pardus''un ilk kararlı sürümü hangi yıl ortaya çıkmıştır?', 
        'A: 2022', 'B: 2018', 'C: 2012', 'D: 2005', 3, '');

insert into LESSONS values (NULL, 6, 'İnternet Tarihi', 'tarih_internet_1', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 12, 0, 0, 'İnternetin atası olarak kabul edilen ve ABD Savunma Bakanlığı tarafından geliştirilen projenin adı nedir?',
        'A: ETHERNET', 'B: NSFNet', 'C: ARPANET', 'D: World Wide Web', 2, ''),
    (NULL, 12, 1, 0, 'World Wide Web'' i (www) icat eden ve ilk web tarayıcısını geliştiren bilim insanı kimdir?', 
        'A: Vint Cerf', 'B: Tim Berners-Lee', 'C: Robert Kahn', 'D: Marc Andreessen', 1, ''),
    (NULL, 12, 2, 0, 'İlk popüler grafiksel web tarayıcısı hangisidir?',
        'A: Internet Explorer', 'B: Netscape Navigator', 'C: Mosaic', 'D: Opera', 2, ''),
    (NULL, 12, 3, 0, 'Google arama motoru hangi yıl kurulmuştur?',
        'A: 1995', 'B: 1998', 'C: 2001', 'D: 2004', 1, '');

insert into LESSONS values (NULL, 1, 'Temel Bilgisayar Kavramları', 'genel_bilgisayar_1', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 13, 0, 0, 'Aşağıdakilerden hangisi bir işletim sistemi değildir?',
        'A: Windows', 'B: Pardus', 'C: Microsoft Office', 'D: macOS', 2, ''),
    (NULL, 13, 1, 0, 'CPU''nun açılımı nedir?', 
        'A: Central Processing Unit', 'B: Computer Personal Unit', 'C: Central Power Unit', 'D: Computer Processing Usage', 0, ''),
    (NULL, 13, 2, 0, 'Bilgilerin kalıcı olarak saklandığı donanım birimi hangisidir?',
        'A: RAM', 'B: CPU', 'C: Ekran Kartı', 'D: Sabit Disk (HDD/SSD)', 3, ''),
    (NULL, 13, 3, 0, 'İnternete bağlanmak için kullanılan cihaza ne ad verilir?',
        'A: Yazıcı', 'B: Modem', 'C: Tarayıcı', 'D: Hoparlör', 1, '');

insert into LESSONS values (NULL, 1, 'Ağ ve İnternet Temelleri', 'genel_bilgisayar_2', 4);
insert into LESSON_QUESTIONS values 
    (NULL, 14, 0, 0, 'Bir web sitesinin internetteki adresi nedir?',
        'A: IP Adresi', 'B: URL (Alan Adı)', 'C: MAC Adresi', 'D: DNS', 1, ''),
    (NULL, 14, 1, 0, 'Aşağıdakilerden hangisi bir web tarayıcısıdır?',
        'A: GIMP', 'B: LibreOffice', 'C: Firefox', 'D: VLC Player', 2, ''),
    (NULL, 14, 2, 0, 'İki veya daha fazla bilgisayarın birbirine bağlanmasıyla oluşan yapıya ne denir?',
        'A: Ağ (Network)', 'B: Veritabanı', 'C: Sunucu', 'D: İşletim Sistemi', 0, ''),
    (NULL, 14, 3, 0, '"Wi-Fi" ne anlama gelir?',
        'A: Geniş Bant', 'B: Kablolu Bağlantı', 'C: Kablosuz Ağ Teknolojisi', 'D: Fiber Optik', 2, '');

insert into LESSONS values (NULL, 3, 'Donanım Bileşenleri Testi', 'donanim_test_1', 5);
insert into LESSON_QUESTIONS values 
    (NULL, 15, 0, 0, 'Bilgisayarın beyni olarak kabul edilen donanım hangisidir?', 
        'A: RAM', 'B: Anakart', 'C: CPU', 'D: Güç Kaynağı', 2, ''),
    (NULL, 15, 1, 0, 'Aşağıdakilerden hangisi bir depolama birimi türü değildir?',
        'A: SSD', 'B: HDD', 'C: USB Bellek', 'D: ALU', 3, ''),
    (NULL, 15, 2, 0, 'Görüntüyü işleyip monitöre gönderen donanım hangisidir?',
        'A: Ses Kartı', 'B: Ekran Kartı', 'C: Ağ Kartı', 'D: TV Kartı', 1, ''),
    (NULL, 15, 3, 0, 'Tüm donanım birimlerinin üzerine takıldığı ve iletişim kurmasını sağlayan bileşen hangisidir?',
        'A: Anakart', 'B: Kasa', 'C: İşlemci', 'D: RAM', 0, ''),
    (NULL, 15, 4, 0, 'Oyun performansını en çok etkileyen donanım hangisidir?', 
        'A: Güç Kaynağı', 'B: Sabit Disk', 'C: Ekran Kartı', 'D: Ses Kartı', 2, '');

insert into LESSONS values (NULL, 4, 'Yazılım Türleri Testi', 'yazilim_test_1', 4);
insert into LESSON_QUESTIONS values
    (NULL, 16, 0, 0, 'Aşağıdakilerden hangisi bir uygulama yazılımıdır?',
        'A: Linux Çekirdeği', 'B: BIOS', 'C: LibreOffice Writer', 'D: Windows Sürücüleri', 2, ''),
    (NULL, 16, 1, 0, 'Kaynak kodları halka açık olan ve herkes tarafından geliştirilebilen yazılımlara ne ad verilir?',
        'A: Ücretli Yazılım', 'B: Kapalı Kaynak Yazılım', 'C: Açık Kaynak Yazılım', 'D: Demo Yazılım', 2, ''),
    (NULL, 16, 2, 0, 'Bilgisayar donanımını yöneten ve uygulama yazılımları için temel hizmetleri sağlayan yazılım hangisidir?',
        'A: İşletim Sistemi', 'B: Antivirüs', 'C: Web Tarayıcı', 'D: Ofis Paketi', 0, ''),
    (NULL, 16, 3, 0, 'Bir yazılımın, insan tarafından okunabilir kaynak kodunu makine diline çeviren programa ne denir?',
        'A: Yorumlayıcı (Interpreter)', 'B: Derleyici (Compiler)', 'C: Hata Ayıklayıcı (Debugger)', 'D: Metin Editörü', 1, '');

