/*
 *
 * Copyright 2022-2023 libtoml authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "src/timezone.h"
#include <unordered_map>

#define ADD_TIMEZONE(N, L) g_timezone_table->insert({#N, #L})

namespace TOML {
namespace {
using TimeZoneTable = std::unordered_multimap<std::string, std::string>;

TimeZoneTable *g_timezone_table = nullptr;
} // namespace

int InitTimeZone() {
    if (g_timezone_table) {
        return static_cast<int>(g_timezone_table->size());
    }
    g_timezone_table = new TimeZoneTable();
    ADD_TIMEZONE(ACDT, Australian);
    ADD_TIMEZONE(ACST, Australian);
    ADD_TIMEZONE(ACT, Acre);
    ADD_TIMEZONE(ACWST, Australian);
    ADD_TIMEZONE(ADT, Atlantic);
    ADD_TIMEZONE(AEDT, Australian);
    ADD_TIMEZONE(AEST, Australian);
    ADD_TIMEZONE(AFT, Afghanistan);
    ADD_TIMEZONE(AKDT, Alaska);
    ADD_TIMEZONE(AKST, Alaska);
    ADD_TIMEZONE(AMST, Amazon);
    ADD_TIMEZONE(AMT, Amazon);
    ADD_TIMEZONE(AMT, Armenia);
    ADD_TIMEZONE(ART, Argentina);
    ADD_TIMEZONE(AST, Atlantic);
    ADD_TIMEZONE(AST, Arabia);
    ADD_TIMEZONE(AT, Atlantic);
    ADD_TIMEZONE(AWST, Australian);
    ADD_TIMEZONE(AZOST, Azores);
    ADD_TIMEZONE(AZOT, Azores);
    ADD_TIMEZONE(AZT, Azerbaijan);
    ADD_TIMEZONE(BDT, Brunei);
    ADD_TIMEZONE(BIT, Baker);
    ADD_TIMEZONE(BNT, Brunei);
    ADD_TIMEZONE(BOT, Bolivia);
    ADD_TIMEZONE(BRST, Brasilia);
    ADD_TIMEZONE(BRT, Brasilia);
    ADD_TIMEZONE(BST, British);
    ADD_TIMEZONE(BST, Bangladesh);
    ADD_TIMEZONE(BST, Bougainville);
    ADD_TIMEZONE(BTT, Bhutan);
    ADD_TIMEZONE(CAT, Central);
    ADD_TIMEZONE(CCT, Cocos);
    ADD_TIMEZONE(CDT, Central);
    ADD_TIMEZONE(CDT, Cuba);
    ADD_TIMEZONE(CEST, Central);
    ADD_TIMEZONE(CET, Central);
    ADD_TIMEZONE(CHADT, Chatham);
    ADD_TIMEZONE(CHAST, Chatham);
    ADD_TIMEZONE(CHOST, Choibalsan);
    ADD_TIMEZONE(CHOT, Choibalsan);
    ADD_TIMEZONE(CHST, Chamorro);
    ADD_TIMEZONE(CHUT, Chuuk);
    ADD_TIMEZONE(CIST, Clipperton);
    ADD_TIMEZONE(CIT, Central);
    ADD_TIMEZONE(CKT, Cook);
    ADD_TIMEZONE(CLST, Chile);
    ADD_TIMEZONE(CLT, Chile);
    ADD_TIMEZONE(COST, Colombia);
    ADD_TIMEZONE(COT, Colombia);
    ADD_TIMEZONE(CST, Central);
    ADD_TIMEZONE(CST, Cuba);
    ADD_TIMEZONE(CST, China);
    ADD_TIMEZONE(CT, Central);
    ADD_TIMEZONE(CVT, Cape);
    ADD_TIMEZONE(CWST, Central);
    ADD_TIMEZONE(CXT, Christmas);
    ADD_TIMEZONE(DAVT, Davis);
    ADD_TIMEZONE(DDUT, Dumont);
    ADD_TIMEZONE(EASST, Easter);
    ADD_TIMEZONE(EAST, Easter);
    ADD_TIMEZONE(EAT, East);
    ADD_TIMEZONE(ECT, Ecuador);
    ADD_TIMEZONE(EDT, Eastern);
    ADD_TIMEZONE(EEST, Eastern);
    ADD_TIMEZONE(EET, Eastern);
    ADD_TIMEZONE(EGST, Eastern);
    ADD_TIMEZONE(EGT, Eastern);
    ADD_TIMEZONE(EIT, Eastern);
    ADD_TIMEZONE(EST, Eastern);
    ADD_TIMEZONE(ET, Eastern);
    ADD_TIMEZONE(FET, Further - eastern);
    ADD_TIMEZONE(FJT, Fiji);
    ADD_TIMEZONE(FKST, Falkland);
    ADD_TIMEZONE(FKT, Falkland);
    ADD_TIMEZONE(FNT, Fernando);
    ADD_TIMEZONE(GALT, Galapagos);
    ADD_TIMEZONE(GAMT, Gambier);
    ADD_TIMEZONE(GET, Georgia);
    ADD_TIMEZONE(GFT, French);
    ADD_TIMEZONE(GILT, Gilbert);
    ADD_TIMEZONE(GIT, Gambier);
    ADD_TIMEZONE(GMT, Greenwich);
    ADD_TIMEZONE(GST, Gulf);
    ADD_TIMEZONE(GST, South);
    ADD_TIMEZONE(GYT, Guyana);
    ADD_TIMEZONE(HADT, Hawaii - Aleutian);
    ADD_TIMEZONE(HAST, Hawaii - Aleutian);
    ADD_TIMEZONE(HKT, Hong);
    ADD_TIMEZONE(HMT, Heard);
    ADD_TIMEZONE(HOVST, Khovd);
    ADD_TIMEZONE(HOVT, Khovd);
    ADD_TIMEZONE(ICT, Indochina);
    ADD_TIMEZONE(IDT, Israel);
    ADD_TIMEZONE(IOT, Indian);
    ADD_TIMEZONE(IRDT, Iran);
    ADD_TIMEZONE(IRKT, Irkutsk);
    ADD_TIMEZONE(IRST, Iran);
    ADD_TIMEZONE(IST, Indian);
    ADD_TIMEZONE(IST, Irish);
    ADD_TIMEZONE(IST, Israel);
    ADD_TIMEZONE(JST, Japan);
    ADD_TIMEZONE(KGT, Kyrgyzstan);
    ADD_TIMEZONE(KOST, Kosrae);
    ADD_TIMEZONE(KRAT, Krasnoyarsk);
    ADD_TIMEZONE(KST, Korea);
    ADD_TIMEZONE(LHDT, Lord);
    ADD_TIMEZONE(LHST, Lord);
    ADD_TIMEZONE(LINT, Line);
    ADD_TIMEZONE(MAGT, Magadan);
    ADD_TIMEZONE(MART, Marquesas);
    ADD_TIMEZONE(MAWT, Mawson);
    ADD_TIMEZONE(MDT, Mountain);
    ADD_TIMEZONE(MHT, Marshall);
    ADD_TIMEZONE(MIST, Macquarie);
    ADD_TIMEZONE(MIT, Marquesas);
    ADD_TIMEZONE(MMT, Myanmar);
    ADD_TIMEZONE(MSK, Moscow);
    ADD_TIMEZONE(MST, Mountain);
    ADD_TIMEZONE(MST, Malaysia);
    ADD_TIMEZONE(MT, Mountain);
    ADD_TIMEZONE(MUT, Mauritius);
    ADD_TIMEZONE(MVT, Maldives);
    ADD_TIMEZONE(MYT, Malaysia);
    ADD_TIMEZONE(NCT, New);
    ADD_TIMEZONE(NDT, Newfoundland);
    ADD_TIMEZONE(NFT, Norfolk);
    ADD_TIMEZONE(NPT, Nepal);
    ADD_TIMEZONE(NRT, Nauru);
    ADD_TIMEZONE(NST, Newfoundland);
    ADD_TIMEZONE(NT, Newfoundland);
    ADD_TIMEZONE(NUT, Niue);
    ADD_TIMEZONE(NZDT, New);
    ADD_TIMEZONE(NZST, New);
    ADD_TIMEZONE(OMST, Omsk);
    ADD_TIMEZONE(ORAT, Oral);
    ADD_TIMEZONE(PDT, Pacific);
    ADD_TIMEZONE(PET, Peru);
    ADD_TIMEZONE(PETT, Kamchatka);
    ADD_TIMEZONE(PGT, Papua);
    ADD_TIMEZONE(PHOT, Phoenix);
    ADD_TIMEZONE(PhST, Philippine);
    ADD_TIMEZONE(PHT, Philippine);
    ADD_TIMEZONE(PKT, Pakistan);
    ADD_TIMEZONE(PMDT, Saint);
    ADD_TIMEZONE(PMST, Saint);
    ADD_TIMEZONE(PONT, Pohnpei);
    ADD_TIMEZONE(PST, Pacific);
    ADD_TIMEZONE(PT, Pacific);
    ADD_TIMEZONE(PWT, Palau);
    ADD_TIMEZONE(PYST, Paraguay);
    ADD_TIMEZONE(PYT, Paraguay);
    ADD_TIMEZONE(RET, R��union);
    ADD_TIMEZONE(ROTT, Rothera);
    ADD_TIMEZONE(SAKT, Sakhalin);
    ADD_TIMEZONE(SAMT, Samara);
    ADD_TIMEZONE(SAST, South);
    ADD_TIMEZONE(SBT, Solomon);
    ADD_TIMEZONE(SCT, Seychelles);
    ADD_TIMEZONE(SGT, Singapore);
    ADD_TIMEZONE(SLST, Sri);
    ADD_TIMEZONE(SRET, Srednekolymsk);
    ADD_TIMEZONE(SRT, Suriname);
    ADD_TIMEZONE(SST, Samoa);
    ADD_TIMEZONE(SYOT, Showa);
    ADD_TIMEZONE(TAHT, Tahiti);
    ADD_TIMEZONE(TFT, French);
    ADD_TIMEZONE(THA, Thailand);
    ADD_TIMEZONE(TJT, Tajikistan);
    ADD_TIMEZONE(TKT, Tokelau);
    ADD_TIMEZONE(TLT, Timor);
    ADD_TIMEZONE(TMT, Turkmenistan);
    ADD_TIMEZONE(TOT, Tonga);
    ADD_TIMEZONE(TRT, Turkey);
    ADD_TIMEZONE(TVT, Tuvalu);
    ADD_TIMEZONE(ULAST, Ulaanbaatar);
    ADD_TIMEZONE(ULAT, Ulaanbaatar);
    ADD_TIMEZONE(USZ1, Kaliningrad);
    ADD_TIMEZONE(UTC, Coordinated);
    ADD_TIMEZONE(UYST, Uruguay);
    ADD_TIMEZONE(UYT, Uruguay);
    ADD_TIMEZONE(UZT, Uzbekistan);
    ADD_TIMEZONE(VET, Venezuelan);
    ADD_TIMEZONE(VLAT, Vladivostok);
    ADD_TIMEZONE(VOLT, Volgograd);
    ADD_TIMEZONE(VOST, Vostok);
    ADD_TIMEZONE(VUT, Vanuatu);
    ADD_TIMEZONE(WAKT, Wake);
    ADD_TIMEZONE(WAST, West);
    ADD_TIMEZONE(WAT, West);
    ADD_TIMEZONE(WEST, Western);
    ADD_TIMEZONE(WET, Western);
    ADD_TIMEZONE(WFT, Wallis);
    ADD_TIMEZONE(WGST, West);
    ADD_TIMEZONE(WGST, West);
    ADD_TIMEZONE(WIB, Western);
    ADD_TIMEZONE(WIT, Eastern);
    ADD_TIMEZONE(WST, Western);
    ADD_TIMEZONE(YAKT, Yakutsk);
    ADD_TIMEZONE(YEKT, Yekaterinburg);
    return static_cast<int>(g_timezone_table->size());
}

bool IsTimeZoneExists(const std::string &name) {
    if (g_timezone_table) {
        return g_timezone_table->count(name) != 0;
    }
    return false;
}

std::vector<std::string> GetTimeZoneLocation(const std::string &name) {
    std::vector<std::string> res;
    if (g_timezone_table) {
        auto range = g_timezone_table->equal_range(name);
        for (auto it = range.first; it != range.second; ++it) {
            res.push_back(it->second);
        }
    }
    return res;
}

void CleanupTimeZone() {
    if (g_timezone_table) {
        delete g_timezone_table;
        g_timezone_table = nullptr;
    }
}
#undef ADD_TIMEZONE
} // namespace TOML
