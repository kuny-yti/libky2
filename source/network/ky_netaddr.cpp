#include "ky_netaddr.h"
#include "ky_memory.h"

#if kyOSIsLinux
# include <arpa/inet.h>
# include <netinet/in.h>
# include <ifaddrs.h>
# include <netdb.h>
# include <unistd.h>
# include <netinet/tcp.h>
#elif kyOSIsWin32

#endif
static bool is_valid_domain_name(const std::string& str)
{
    static const std::string tlds="|com|net|org|edu|gov|int|mil|ac|ad|ae|af|ag|ai|al|am|an|ao|aq|ar|arpa|at|au|ax|az|ba|bb|bd|be|bf|bg|bh|bi|bj|bm|bn|bo|br|bs|bt|bv|bw|by|bz|ca|cc|cd|cf|cg|ch|ci|ck|cl|cm|cn|co|cr|cu|cv|cx|cy|cz|de|dj|dk|dm|do|dz|ec|ee|eg|er|es|et|eu|fi|fj|fk|fm|fo|fr|ga|gb|gd|ge|gf|gg|gh|gi|gl|gm|gn|gp|gq|gr|gs|gt|gu|gw|gy|hk|hm|hn|hr|ht|hu|id|ie|il|im|in|io|iq|ir|is|it|je|jm|jo|jp|ke|kg|kh|ki|km|kn|kp|kr|kw|ky|kz|la|lb|lc|li|lk|lr|ls|lt|lu|lv|ly|ma|mc|md|me|mg|mh|mk|ml|mm|mn|mo|mp|mq|ms|mt|mu|mv|mw|mx|my|mz|na|nc|ne|nf|ng|ni|nl|no|np|nr|nu|nz|om|pa|pe|pf|pg|ph|pk|pl|pm|pn|pr|ps|pt|pw|py|qa|re|ro|rs|ru|rw|sa|sb|sc|sd|se|sg|sh|si|sj|sk|sl|sm|sn|so|sr|st|su|sv|sy|sz|tc|td|tf|tg|th|tj|tk|tl|tm|tn|to|tp|tr|tt|tv|tw|tz|ua|ug|uk|us|uy|uz|va|vc|ve|vg|vi|vn|vu|wf|ws|ye|yt|za|zm|zw|aw|as|mr|bl|bq|cw|eh|mf|ss|sx|um|aaa|aarp|abarth|abb|abbott|abbvie|abc|able|abogado|abudhabi|academy|accenture|accountant|accountants|acer|aco|active|actor|adac|ads|adult|aeg|aero|aetna|afamilycompany|afl|africa|africamagic|agakhan|agency|aig|aigo|airbus|airforce|airtel|akdn|alcon|alfaromeo|alibaba|alipay|allfinanz|allfinanzberater|allfinanzberatung|allstate|ally|alsace|alstom|amazon|americanexpress|americanfamily|amex|amfam|amica|amp|amsterdam|analytics|and|android|anquan|ansons|anthem|antivirus|aol|apartments|app|apple|aquarelle|aquitaine|arab|aramco|archi|architect|are|army|art|arte|asda|asia|associates|astrium|athleta|attorney|auction|audi|audible|audio|auspost|author|auto|autoinsurance|autos|avery|avianca|axa|axis|azure|baby|baidu|banamex|bananarepublic|band|bank|banque|bar|barcelona|barclaycard|barclays|barefoot|bargains|baseball|basketball|bauhaus|bayern|bbb|bbc|bbt|bbva|bcg|bcn|beats|beauty|beer|beknown|bentley|berlin|best|bestbuy|bet|bharti|bible|bid|bike|bing|bingo|bio|biz|black|blackfriday|blanco|blockbuster|blog|bloomberg|bloomingdales|blue|bms|bmw|bnl|bnpparibas|boats|boehringer|bofa|bom|bond|boo|book|booking|boots|bosch|bostik|boston|bot|boutique|box|bradesco|bridgestone|broadway|broker|brother|brussels|budapest|bugatti|buick|build|builders|business|buy|buzz|bway|bzh|cab|cadillac|cafe|cal|call|calvinklein|cam|camera|camp|canalplus|cancerresearch|canon|capetown|capital|capitalone|caravan|cards|care|career|careers|caremore|carinsurance|cars|cartier|casa|case|caseih|cash|cashbackbonus|casino|cat|catalonia|catering|catholic|cba|cbn|cbre|cbs|ceb|center|ceo|cern|cfa|cfd|chanel|changiairport|channel|charity|chartis|chase|chat|chatr|cheap|chesapeake|chevrolet|chevy|chintai|city|chk|chloe|christmas|chrome|chrysler|church|cialis|cimb|cipriani|circle|cisco|citadel|citi|citic|cityeats|claims|cleaning|click|clinic|clinique|clothing|club|clubmed|coach|codes|coffee|college|cologne|com|comcast|commbank|community|company|compare|computer|comsec|condos|connectors|construction|consulting|contact|contractors|cooking|cookingchannel|cool|coop|corsica|country|coupon|coupons|courses|credit|creditcard|creditunion|cricket|crown|crs|cruise|cruises|csc|cuisinella|cymru|cyou|dabur|dad|dance|data|date|dating|datsun|day|dclk|dds|deal|dealer|deals|degree|delivery|dell|delmonte|deloitte|delta|democrat|dental|dentist|desi|design|deutschepost|dhl|diamonds|diet|digikey|digital|direct|directory|discount|discover|dish|diy|dnb|dnp|docomo|docs|doctor|dodge|dog|doha|domains|doosan|dot|dotafrica|download|drive|dstv|dtv|dubai|duck|dunlop|duns|dupont|durban|dvag|dvr|dwg|earth|eat|eco|ecom|edeka|education|edu|email|emerck|emerson|energy|engineer|engineering|enterprises|epost|epson|equipment|ericsson|erni|esq|est|estate|esurance|etisalat|eurovision|eus|events|everbank|exchange|expert|exposed|express|extraspace|fage|fail|fairwinds|faith|family|fan|fans|farm|farmers|fashion|fast|fedex|feedback|ferrari|ferrero|fiat|fidelity|fido|film|final|finance|financial|financialaid|finish|fire|firestone|firmdale|fish|fishing|fitness|flickr|flights|flir|florist|flowers|fls|flsmidth|fly|foo|food|foodnetwork|football|ford|forex|forsale|forum|foundation|fox|free|fresenius|frl|frogans|frontdoor|frontier|ftr|fujitsu|fujixerox|fun|fund|furniture|futbol|fyi|gai|gal|gallery|gallo|gallup|game|games|gap|garden|garnier|gay|gbiz|gcc|gdn|gea|gecompany|ged|gent|genting|george|ggee|gift|gifts|gives|giving|glade|glass|gle|glean|global|globalx|globo|gmail|gmbh|gmc|gmo|gmx|godaddy|gold|goldpoint|golf|goo|goodhands|goodyear|goog|google|gop|got|gotv|grainger|graphics|gratis|gree|green|gripe|grocery|group|guardian|guardianlife|guardianmedia|gucci|guge|guide|guitars|guru|hair|halal|hamburg|hangout|haus|hbo|hdfc|hdfcbank|health|healthcare|heart|heinz|help|helsinki|here|hermes|hgtv|hilton|hiphop|hisamitsu|hitachi|hiv|hkt|hockey|holdings|holiday|homedepot|homegoods|homes|homesense|honda|honeywell|horse|host|hosting|hoteis|hotel|hoteles|hotels|hotmail|house|how|htc|hughes|hyatt|hyundai|ibm|icbc|ice|icu|idn|ieee|ifm|iinet|ikano|imamat|imdb|immo|immobilien|indians|industries|infiniti|info|infosys|infy|ing|ink|institute|insurance|insure|intel|international|intuit|investments|ipiranga|ira|irish|iselect|islam|ismaili|ist|istanbul|itau|itv|iveco|iwc|jaguar|java|jcb|jcp|jeep|jetzt|jewelry|jio|jlc|jll|jmp|jnj|jobs|joburg|jot|joy|jpmorgan|jpmorganchase|jprs|juegos|juniper|justforu|kaufen|kddi|kerastase|kerryhotels|kerrylogisitics|kerryproperties|ketchup|kfh|kia|kid|kids|kiehls|kim|kinder|kindle|kitchen|kiwi|koeln|komatsu|konami|kone|kosher|kpmg|kpn|krd|kred|ksb|kuokgroup|kyknet|kyoto|lacaixa|ladbrokes|lamborghini|lamer|lancaster|lancia|lancome|land|landrover|lanxess|lat|latino|latrobe|lawyer|lds|lease|leclerc|lefrak|legal|lego|lexus|lgbt|liaison|lidl|life|lifeinsurance|lifestyle|lighting|lightning|like|lilly|limited|limo|lincoln|linde|link|lipsy|live|livestrong|living|lixil|llc|loan|loans|locker|locus|loft|lol|london|loreal|lotte|lotto|love|lpl|lplfinancial|ltd|ltda|lundbeck|lupin|luxe|luxury|macys|madrid|maif|maison|makeup|man|management|mango|map|market|marketing|markets|marriott|marshalls|maserati|mattel|maybelline|mba|mcd|mcdonalds|mckinsey|media|medical|meet|melbourne|meme|memorial|men|menu|meo|merck|merckmsd|metlife|miami|microsoft|mih|mii|mini|mint|mit|mitek|mitsubishi|mlb|mls|mnet|mobi|mobile|mobily|moda|moe|mom|monash|money|monster|montblanc|mopar|mormon|mortgage|moscow|moto|motorcycles|mov|movie|movistar|mozaic|mrmuscle|mrporter|mtn|mtpc|mtr|multichoice|museum|music|mutual|mutualfunds|mutuelle|mzansimagic|nab|nadex|nagoya|name|naspers|nationwide|natura|navy|nba|nec|net|netaporter|netbank|netflix|network|neustar|new|newholland|news|next|nextdirect|nexus|nfl|ngo|nhk|nico|nike|nikon|ninja|nissan|nissay|nokia|northlandinsurance|northwesternmutual|norton|now|nowruz|nowtv|nra|nrw|ntt|nyc|obi|observer|off|okinawa|olayan|olayangroup|oldnavy|ollo|olympus|omega|ong|onl|online|onyourside|ooo|open|oracle|orange|org|organic|orientexpress|origins|osaka|otsuka|ott|overheidnl|ovh|page|pamperedchef|panasonic|panerai|paris|pars|partners|parts|party|passagens|patagonia|patch|pay|payu|pccw|persiangulf|pets|pfizer|pharmacy|phd|philips|phone|photo|photography|photos|physio|piaget|pics|pictet|pictures|pid|pin|ping|pink|pioneer|piperlime|pitney|pizza|place|play|playstation|plumbing|plus|pnc|pohl|poker|politie|polo|porn|post|pramerica|praxi|press|prime|pro|prod|productions|prof|progressive|promo|properties|property|protection|pru|prudential|pub|pwc|qpon|qtel|quebec|quest|qvc|racing|radio|raid|ram|read|realestate|realtor|realty|recipes|red|redken|redstone|redumbrella|rehab|reise|reisen|reit|ren|rent|rentals|repair|report|republican|rest|restaurant|retirement|review|reviews|rexroth|rich|richardli|ricoh|rightathome|ril|rio|rip|rmit|rocher|rocks|rockwool|rodeo|rogers|roma|room|rsvp|rugby|ruhr|run|rwe|ryukyu|saarland|safe|safety|safeway|sakura|sale|salon|samsclub|samsung|sandvik|sandvikcoromant|sanofi|sap|sapo|sapphire|sarl|sas|save|saxo|sbi|sbs|sca|scb|schaeffler|schedule|schmidt|scholarhips|scholarships|schule|schwarz|schwarzgroup|science|scjohnson|scor|scot|search|seat|security|seek|select|sener|services|ses|seven|sew|sex|sexy|sfr|shangrila|sharp|shell|shia|shiksha|shirriam|shoes|shop|shopping|shopyourway|shouji|show|showtime|shriram|silk|sina|singles|ski|skin|skolkovo|sky|skydrive|skype|sling|smart|smile|sncf|soccer|social|softbank|software|sohu|solar|solutions|song|sony|soy|spa|space|spiegel|sport|sports|spot|spreadbetting|srt|stada|staples|star|starhub|statebank|statefarm|statoil|stc|stcgroup|stockholm|storage|store|stream|stroke|studio|study|style|sucks|supersport|supplies|supply|support|surf|surgery|suzuki|svr|swatch|swiftcover|swiss|symantec|systems|sydney|tab|taipei|talk|taobao|target|tata|tatamotors|tatar|tattoo|tax|taxi|tci|tdk|team|technology|tel|telecity|telefonica|temasek|tennis|terra|teva|thai|thd|theater|theatre|theguardian|thehartford|tiaa|tickets|tienda|tiffany|tips|tires|tirol|tjmaxx|tjx|tkmaxx|tmall|today|tokyo|tools|top|toray|toshiba|total|tour|tours|town|toyota|toys|trade|tradershotels|trading|training|transformers|translations|transunion|travel|travelchannel|travelers|travelersinsurance|travelguard|trust|trv|tube|tui|tunes|tushu|tvs|ubank|ubs|uconnect|ultrabook|ummah|unicom|unicorn|university|uno|uol|ups|vacations|vana|vanguard|vanish|vegas|ventures|verisign|versicherung|vet|viajes|video|vig|viking|villas|vin|vip|virgin|visa|vision|vista|vistaprint|viva|vivo|vlaanderen|vodka|volkswagen|volvo|vons|vote|voting|voto|voyage|vuelos|wales|walmart|walter|wang|wanggou|warman|watch|watches|weather|weatherchannel|web|webcam|weber|webjet|webs|website|wed|wedding|weibo|weir|whoswho|wien|wiki|williamhill|wilmar|windows|wine|winners|wme|wolterskluwer|world|woodside|work|works|wow|wtc|wtf|xbox|xerox|xfinity|xihuan|xin|xn--11b4c3d|xn--1ck2e1b|xn--1qqw23a|xn--30rr7y|xn--3bst00m|xn--3ds443g|xn--3e0b707e|xn--3oq18vl8pn36a|xn--3pxu8k|xn--42c2d9a|xn--45brj9c|xn--45q11c|xn--4gbrim|xn--4gq48lf9j|xn--54b7fta0cc|xn--55qw42g|xn--55qx5d|xn--55qx5d8y0buji4b870u|xn--5su34j936bgsg|xn--5tzm5g|xn--6frz82g|xn--6qq986b3x1|xn--6qq986b3xl|xn--6rtwn|xn--80adxhks|xn--80ao21a|xn--80aqecdr1a|xn--80asehdb|xn--80aswg|xn--8y0a063a|xn--90a3ac|xn--9et52u|xn--9krt00a|xn--b4w605ferd|xn--bck1b9a5dre4c|xn--c1avg|xn--c1yn36f|xn--c2br7g|xn--cck2b3b|xn--cckwcxetd|xn--cg4bki|xn--clchc0ea0b2g2a9gcd|xn--czr694b|xn--czrs0t|xn--czru2d|xn--d1acj3b|xn--dkwm73cwpn|xn--eckvdtc9d|xn--efvy88h|xn--estv75g|xn--fct429k|xn--fes124c|xn--fhbei|xn--fiq228c5hs|xn--fiq64b|xn--fiQ64b|xn--fiQ64B|xn--fiqs8s|xn--fiqz9s|xn--fjq720a|xn--flw351e|xn--fpcrj9c3d|xn--fzc2c9e2c|xn--fzys8d69uvgm|xn--g2xx48c|xn--gckr3f0f|xn--gecrj9c|xn--gk3at1e|xn--h2brj9c|xn--hdb9cza1b|xn--hxt035cmppuel|xn--hxt035czzpffl|xn--hxt814e|xn--i1b6b1a6a2e|xn--imr513n|xn--io0a7i|xn--j1aef|xn--j1amh|xn--j6w193g|xn--j6w470d71issc|xn--jlq480n2rg|xn--jlq61u9w7b|xn--jvr189m|xn--kcrx77d1x4a|xn--kcrx7bb75ajk3b|xn--kprw13d|xn--kpry57d|xn--kpu716f|xn--kput3i|xn--lgbbat1ad8j|xn--mgb9awbf|xn--mgba3a3ejt|xn--mgba3a4f16a|xn--mgba7c0bbn0a|xn--mgbaakc7dvf|xn--mgbaam7a8h|xn--mgbab2bd|xn--mgbai9azgqp6j|xn--mgbayh7gpa|xn--mgbb9fbpob|xn--mgbbh1a71e|xn--mgbc0a9azcg|xn--mgbca7dzdo|xn--mgberp4a5d4ar|xn--mgbi4ecexp|xn--mgbt3dhd|xn--mgbv6cfpo|xn--mgbx4cd0ab|xn--mk1bu44c|xn--mxtq1m|xn--ngbc5azd|xn--ngbe9e0a|xn--ngbrx|xn--node|xn--nqv7f|xn--nqv7fs00ema|xn--nyqy26a|xn--o3cw4h|xn--ogbpf8fl|xn--otu796d|xn--p1acf|xn--p1ai|xn--pbt977c|xn--pgb3ceoj|xn--pgbs0dh|xn--pssy2u|xn--q9jyb4c|xn--qcka1pmc|xn--rhqv96g|xn--rovu88b|xn--s9brj9c|xn--ses554g|xn--t60b56a|xn--tckwe|xn--tiq49xqyj|xn--tqq33ed31aqia|xn--unup4y|xn--vermgensberater-ctb|xn--vermgensberatung-pwb|xn--vhquv|xn--vuq861b|xn--w4r85el8fhu5dnra|xn--w4rs40l|xn--wgbh1c|xn--wgbl6a|xn--xhq521b|xn--xkc2al3hye2a|xn--xkc2dl3a5ee0h|xn--yfro4i67o|xn--ygbi2ammx|xn--zfr164b|xn—3ds443g|xn—fiq228c5hs|xperia|xxx|xyz|yachts|yahoo|yamaxun|yandex|yellowpages|yodobashi|yoga|yokohama|you|youtube|yun|zappos|zara|zero|zip|zippo|zone|zuerich|zulu|";

    const char* s = str.c_str();
    int segsize = 0,i,ix = str.length();
    if (s[0]=='.' || s[ix-1]=='.' || ix > 253)
        return false;
    for(i = 0, segsize = 0; i < ix; i++)
    {
        if (s[i]=='.')
        {
            if (segsize == 0) //fail for abc..com
                return false;
            segsize = 0;
        }
        else if ( ('0' <= s[i] && s[i] <='9') || ('a' <= s[i] && s[i] <='z') ||
                  ('A' <= s[i] && s[i] <='Z') ||
                  (s[i]=='-' && segsize != 0 && i+1 < ix && s[i+1]!='.'))
        {
            segsize++;
        }
        else
            return false; //invalid char...

        if (segsize > 63)
            return false;
    }

    std::stringstream ss;
    ss << "|" << str.substr(ix - segsize) << "|" ;//get last domain segment

    if (tlds.find(ss.str()) == std::string::npos)
        return false;

    return true;
}

static bool legality_ipv4(const ky_string &ipaddr)
{
    struct sockaddr_in sa;
    char *ips = ipaddr.to_latin1 ();
    return inet_pton(AF_INET, ips, &(sa.sin_addr)) != 0;
}
static bool legality_ipv6(const ky_string &ipaddr)
{
    struct sockaddr_in6 sa;
    char *ips = ipaddr.to_latin1 ();
    return inet_pton(AF_INET6, ips, &(sa.sin6_addr))!=0;
}

static ky_list<ky_string> dns_lookup(const ky_string &host_name, int ipv)
{
    ky_list<ky_string> output;

    struct addrinfo hints, *res, *p;
    int status, ai_family;
    char ip_address[INET6_ADDRSTRLEN];

    ai_family = ipv == 6 ? AF_INET6 : AF_INET; //v4 vs v6?
    ai_family = ipv == 0 ? AF_UNSPEC : ai_family; // AF_UNSPEC (any), or chosen
    ky_memory::zero (&hints, sizeof hints);
    hints.ai_family = ai_family;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = ::getaddrinfo(host_name.to_latin1 (), 0, &hints, &res)) != 0) {
        //cerr << "getaddrinfo: "<< gai_strerror(status) << endl;
        return output;
    }

    for(p = res;p != 0; p = p->ai_next)
    {
        void *addr;
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        ::inet_ntop(p->ai_family, addr, ip_address, sizeof ip_address);
        output.push_back(ip_address);
    }
    ::freeaddrinfo(res);
    return output;
}

ky_string ky_netaddr::local_addr()
{
#if !kyOSIsWin32
        struct ifconf ifc;
        struct ifreq  buf[16];

        int sd = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (-1 == sd)
            return ky_string("127.0.0.1");

        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (::ioctl(sd, SIOCGIFCONF, (char *)&ifc))
            return ky_string("127.0.0.1");
        int intr = ifc.ifc_len / sizeof(struct ifreq);
        while (intr-- > 0 && ::ioctl(sd, SIOCGIFADDR, (char *)&buf[intr]));
        ::close(sd);

        return ky_string(::inet_ntoa(((struct sockaddr_in*)(&buf[intr].ifr_addr))->sin_addr));
#else
        //1.初始化wsa
        WSADATA wsaData;
        int ret = ::WSAStartup(MAKEWORD(2,2), &wsaData);
        if (ret != 0)
            return ky_string("127.0.0.1");

        //2.获取主机名
        char hostname[256] = {0};
        ky_memory::zero (hostname, 256);
        ret = ::gethostname(hostname ,sizeof(hostname));
        if (ret == SOCKET_ERROR)
            return ky_string("127.0.0.1");

        //3.获取主机ip
        HOSTENT* host = ::gethostbyname(hostname);
        if (host == 0)
            return ky_string("127.0.0.1");

        ky_string ipv4_str;
        while(*(host->h_addr_list) != 0) //输出ipv4地址
        {
            ipv4_str.append(ky_string(::inet_ntoa(*(struct in_addr *) *host->h_addr_list))) ;
            host->h_addr_list++;
        }

        struct addrinfo hint;
        hint.ai_family = AF_INET6;        //hint 的限定设置
        hint.ai_socktype = SOCK_STREAM;   //这里可是设置 socket type    比如  SOCK_DGRAM
        hint.ai_flags = AI_PASSIVE;       // flags 的标志很多。常用的有AI_CANONNAME;
        hint.ai_protocol = 0;             //设置协议  一般为0，默认
        hint.ai_addrlen = 0;              //下面不可以设置，为0，或者为NULL
        hint.ai_canonname = 0;
        hint.ai_addr = 0;
        hint.ai_next = 0;
        char *port = "3294";
        struct addrinfo *ailist, *aip;
        struct sockaddr_in6 *sinp6;
        int ilRc = ::getaddrinfo(hostname, port, &hint, &ailist); //通过主机名获得地址信息
        if (ilRc < 0 || ailist == 0)
            return ipv4_str;

        ky_string ipv6_str;
        for (aip = ailist; aip != 0; aip = aip->ai_next) //显示获取的信息
        {
            aip->ai_family == AF_INET6;
            sinp6 = (struct sockaddr_in6 *)aip->ai_addr;

            ky_string new_ipv6;
            for(int i = 0; i < 16; i++)
            {
                if(((i -1) %2) && (i > 0))
                {
                    new_ipv6.append(":");
                }
                ky_string tmp;
                tmp.format("%02x", sinp6->sin6_addr.u.Byte[i]);
                new_ipv6.append(tmp);
            }
            ipv6_str.append(new_ipv6);
        }
        ::WSACleanup();
        return ipv4_str + ipv6_str;
#endif
}

bool ky_netaddr::is_legality(const ky_string &ip)
{
    if (legality_ipv4(ip))
        return true;
    else if (legality_ipv6 (ip))
        return true;
    return false;
}

bool ky_netaddr::is_domain(const ky_string &domain)
{
    return is_valid_domain_name(domain.to_std());
}

ky_netaddr::ky_netaddr  (u16 port, bool is_ipv6)
{
    ky_memory::zero (&addr, sizeof(uAddr));
    if ((port != 0) && !is_ipv6)
    {
        addr.ipv4.sin_family = AF_INET;
        addr.ipv4.sin_port = ::htons(port);
        addr.ipv4.sin_addr.s_addr = INADDR_ANY;
    }
    else if (port != 0)
    {
        addr.ipv6.sin6_family = AF_INET6;
        addr.ipv6.sin6_port = ::htons(port);
        addr.ipv6.sin6_addr = in6addr_any;
    }
}
ky_netaddr::ky_netaddr (const ky_netaddr &rhs)
{
    ky_memory::copy (&addr, rhs.socket (), rhs.socklen ());
}
ky_netaddr::ky_netaddr (const sockaddr *sa, int sa_len)
{
    ky_memory::zero (&addr, sizeof(addr));
    if (sa && sa_len)
    {
        if ((sa->sa_family == AF_INET) && (sa_len >= sizeof (addr.ipv4)))
            ky_memory::copy (&addr.ipv4, sa, sizeof (addr.ipv4));
        else if ((sa->sa_family == AF_INET6) && (sa_len >= sizeof (addr.ipv6)))
            ky_memory::copy (&addr.ipv6, sa, sizeof (addr.ipv6));
    }
}
ky_netaddr::ky_netaddr (const ky_string &ip, u16 port )
{
    ky_memory::zero (&addr, sizeof(addr));
    if (legality_ipv6(ip))
    {
        addr.ipv6.sin6_family = AF_INET6;
        addr.ipv6.sin6_port = ::htons(port);
        ::inet_pton(AF_INET6, (char*)ip.to_latin1 (), (uint*)&addr.ipv6.sin6_addr);
    }
    else if (legality_ipv4(ip))
    {
        addr.ipv4.sin_family = AF_INET;
        addr.ipv4.sin_port = ::htons(port);
        addr.ipv4.sin_addr.s_addr = ::inet_addr((char*)ip.to_latin1 ());
    }
}
ky_netaddr::ky_netaddr(u32 ipHostByteOrder, u16 port)
{
    ky_memory::zero (&addr, sizeof(addr));
    addr.ipv4.sin_family = AF_INET;
    addr.ipv4.sin_port = ::htons(port);
    addr.ipv4.sin_addr.s_addr = ::htonl(ipHostByteOrder);
}
ky_netaddr::~ky_netaddr()
{

}

ky_string ky_netaddr::address()const
{
    if (addr.generic.sa_family == AF_INET)
        return ::inet_ntoa(addr.ipv4.sin_addr);
    else if (addr.generic.sa_family == AF_INET6)
    {
        static char buf[128] = {0};
        return ::inet_ntop(AF_INET6, (uint*)&addr.ipv6.sin6_addr, buf, 128);
    }
    return ky_string();
}

u16 ky_netaddr::port()const
{
    if (addr.generic.sa_family == AF_INET)
        return ::ntohs(addr.ipv4.sin_port);
    else if (addr.generic.sa_family == AF_INET6)
        return ::ntohs(addr.ipv6.sin6_port);
    return 0;
}
int ky_netaddr::socklen()const
{
    if (addr.generic.sa_family == AF_INET)
        return sizeof(sockaddr_in);
    else if (addr.generic.sa_family == AF_INET6)
        return sizeof(sockaddr_in6);
    return 0;
}
sockaddr *ky_netaddr::socket()const
{
    if (addr.generic.sa_family == AF_INET)
        return (sockaddr*)&addr.ipv4;
    else if (addr.generic.sa_family == AF_INET6)
        return (sockaddr*)&addr.ipv6;
    return 0;
}
bool ky_netaddr::is_valid()const
{
    if (addr.generic.sa_family == AF_INET)
        return addr.ipv4.sin_port != 0;
    else if (addr.generic.sa_family == AF_INET6)
        return addr.ipv6.sin6_port != 0;
    return false;
}
bool ky_netaddr::is_ipv4()const
{
    return addr.generic.sa_family == AF_INET;
}
bool ky_netaddr::is_ipv6()const
{
    return addr.generic.sa_family == AF_INET6;
}

bool ky_netaddr::is_any()const
{
    if (is_ipv4 () && (addr.ipv4.sin_port != 0))
        return addr.ipv4.sin_addr.s_addr == INADDR_ANY;
    else if (is_ipv6 () && addr.ipv6.sin6_port != 0)
        return !memcmp (&addr.ipv6.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    return false;
}
bool ky_netaddr::operator == (const ky_netaddr &i)
{
    return !memcmp (&addr, &i.addr, sizeof(addr));
}
ky_netaddr &ky_netaddr::operator=(const ky_netaddr &i)
{
    ky_memory::copy (&addr, &i.addr, sizeof(addr));
    return *this;
}
ulong ky_netaddr::scope_id() const
{
    return addr.ipv6.sin6_scope_id;
}
ulong ky_netaddr::flow_info() const
{
    return addr.ipv6.sin6_flowinfo;
}
