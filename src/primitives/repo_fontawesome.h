/**
 *  Copyright (C) 2014 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REPO_FONT_AWESOME_H
#define REPO_FONT_AWESOME_H

//------------------------------------------------------------------------------
#include <QIcon>
#include <QIconEngine>
#include <QPainter>
#include <QRect>
#include <QVariantMap>
#include <QPalette>
#include <QWidget>
#include <QBrush>
#include <QRawFont>
#include <QPainterPath>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
//------------------------------------------------------------------------------
#include <iostream>

namespace repo {
namespace gui {

//------------------------------------------------------------------------------
/*!
 * \brief The RepoFontAwesome class
 * Singleton class that defines the Font Awesome icons.
 */
class RepoFontAwesome : public QObject
{
	Q_OBJECT

private :

	//! Singleton constructor.
	RepoFontAwesome(QObject *parent = 0);

	//! Singleton destructor.
    ~RepoFontAwesome() {}

	//! Singleton copy constructor.
	RepoFontAwesome(const RepoFontAwesome &);

	//! Singleton comparison.
	void operator = (RepoFontAwesome const&);

public:

    //--------------------------------------------------------------------------
	//
    // Getters
	//
    //--------------------------------------------------------------------------

    //! Returns a singleton instance of the RepoFontAwesome class.
    static RepoFontAwesome &getInstance();

    //--------------------------------------------------------------------------

    //! Returns pre-defined server icon.
    static QIcon getHostIcon()
    { return getIconStatic(RepoFontAwesome::fa_laptop); }

    //! Returns pre-defined branch icon.
    static QIcon getBranchIcon()
    { return getIconStatic(RepoFontAwesome::fa_code_fork); }

    //! Returns pre-defined database icon.
    static QIcon getDatabaseIcon()
    { return getIconStatic(RepoFontAwesome::fa_database); }

    //! Returns pre-defined project icon.
    static QIcon getProjectIcon()
    { return getIconStatic(RepoFontAwesome::fa_list_alt); }

    //--------------------------------------------------------------------------

	QIcon getIcon(int iconOn);
    QIcon getIcon(int iconOn, const QColor &color);
	QIcon getIcon(int iconOn, int iconOff);

    //! Returns icon in a static way.
    static QIcon getIconStatic(int iconOn)
    { return RepoFontAwesome::getInstance().getIcon(iconOn); }

    /*!
     * Returns a font with a given size in pixels.
     */
    QFont getFont(int size);

	//! Returns a pointer to the raw font for glyphs extraction.
    QRawFont *getRawFont();

	//! Returns a glyph index in raw font for given icon unicode character.
	quint32 getGlyphIndex(int icon);

	//! Returns a painter path for given icon unicode character.
	QPainterPath getPainterPath(int icon);

    //! Returns the font-name that is used as an icon-map
    inline QString getFontName() { return fontName; }

	static QColor COLOR_NORMAL;
	static QColor COLOR_DISABLED;
	static QColor COLOR_ACTIVE;
	static QColor COLOR_SELECTED;

private:

    QString fontName; //!< Global font name

    QRawFont *rawFont; //! Raw font to extract individual glyphs

public :
	
//------------------------------------------------------------------------------
enum Icons 
{
    fa_adjust = 0xf042,
    fa_adn = 0xf170,
    fa_align_center = 0xf037,
    fa_align_justify = 0xf039,
    fa_align_left = 0xf036,
    fa_align_right = 0xf038,
    fa_ambulance = 0xf0f9,
    fa_anchor = 0xf13d,
    fa_android = 0xf17b,
    fa_angellist = 0xf209,
    fa_angle_double_down = 0xf103,
    fa_angle_double_left = 0xf100,
    fa_angle_double_right = 0xf101,
    fa_angle_double_up = 0xf102,
    fa_angle_down = 0xf107,
    fa_angle_left = 0xf104,
    fa_angle_right = 0xf105,
    fa_angle_up = 0xf106,
    fa_apple = 0xf179,
    fa_archive = 0xf187,
    fa_area_chart = 0xf1fe,
    fa_arrow_circle_down = 0xf0ab,
    fa_arrow_circle_left = 0xf0a8,
    fa_arrow_circle_o_down = 0xf01a,
    fa_arrow_circle_o_left = 0xf190,
    fa_arrow_circle_o_right = 0xf18e,
    fa_arrow_circle_o_up = 0xf01b,
    fa_arrow_circle_right = 0xf0a9,
    fa_arrow_circle_up = 0xf0aa,
    fa_arrow_down = 0xf063,
    fa_arrow_left = 0xf060,
    fa_arrow_right = 0xf061,
    fa_arrow_up = 0xf062,
    fa_arrows = 0xf047,
    fa_arrows_alt = 0xf0b2,
    fa_arrows_h = 0xf07e,
    fa_arrows_v = 0xf07d,
    fa_asterisk = 0xf069,
    fa_at = 0xf1fa,
    fa_automobile = 0xf1b9,
    fa_backward = 0xf04a,
    fa_ban = 0xf05e,
    fa_bank = 0xf19c,
    fa_bar_chart = 0xf080,
    fa_bar_chart_o = 0xf080,
    fa_barcode = 0xf02a,
    fa_bars = 0xf0c9,
    fa_bed = 0xf236,
    fa_beer = 0xf0fc,
    fa_behance = 0xf1b4,
    fa_behance_square = 0xf1b5,
    fa_bell = 0xf0f3,
    fa_bell_o = 0xf0a2,
    fa_bell_slash = 0xf1f6,
    fa_bell_slash_o = 0xf1f7,
    fa_bicycle = 0xf206,
    fa_binoculars = 0xf1e5,
    fa_birthday_cake = 0xf1fd,
    fa_bitbucket = 0xf171,
    fa_bitbucket_square = 0xf172,
    fa_bitcoin = 0xf15a,
    fa_bold = 0xf032,
    fa_bolt = 0xf0e7,
    fa_bomb = 0xf1e2,
    fa_book = 0xf02d,
    fa_bookmark = 0xf02e,
    fa_bookmark_o = 0xf097,
    fa_briefcase = 0xf0b1,
    fa_btc = 0xf15a,
    fa_bug = 0xf188,
    fa_building = 0xf1ad,
    fa_building_o = 0xf0f7,
    fa_bullhorn = 0xf0a1,
    fa_bullseye = 0xf140,
    fa_bus = 0xf207,
    fa_buysellads = 0xf20d,
    fa_cab = 0xf1ba,
    fa_calculator = 0xf1ec,
    fa_calendar = 0xf073,
    fa_calendar_o = 0xf133,
    fa_camera = 0xf030,
    fa_camera_retro = 0xf083,
    fa_car = 0xf1b9,
    fa_caret_down = 0xf0d7,
    fa_caret_left = 0xf0d9,
    fa_caret_right = 0xf0da,
    fa_caret_square_o_down = 0xf150,
    fa_caret_square_o_left = 0xf191,
    fa_caret_square_o_right = 0xf152,
    fa_caret_square_o_up = 0xf151,
    fa_caret_up = 0xf0d8,
    fa_cart_arrow_down = 0xf218,
    fa_cart_plus = 0xf217,
    fa_cc = 0xf20a,
    fa_cc_amex = 0xf1f3,
    fa_cc_discover = 0xf1f2,
    fa_cc_mastercard = 0xf1f1,
    fa_cc_paypal = 0xf1f4,
    fa_cc_stripe = 0xf1f5,
    fa_cc_visa = 0xf1f0,
    fa_certificate = 0xf0a3,
    fa_chain = 0xf0c1,
    fa_chain_broken = 0xf127,
    fa_check = 0xf00c,
    fa_check_circle = 0xf058,
    fa_check_circle_o = 0xf05d,
    fa_check_square = 0xf14a,
    fa_check_square_o = 0xf046,
    fa_chevron_circle_down = 0xf13a,
    fa_chevron_circle_left = 0xf137,
    fa_chevron_circle_right = 0xf138,
    fa_chevron_circle_up = 0xf139,
    fa_chevron_down = 0xf078,
    fa_chevron_left = 0xf053,
    fa_chevron_right = 0xf054,
    fa_chevron_up = 0xf077,
    fa_child = 0xf1ae,
    fa_circle = 0xf111,
    fa_circle_o = 0xf10c,
    fa_circle_o_notch = 0xf1ce,
    fa_circle_thin = 0xf1db,
    fa_clipboard = 0xf0ea,
    fa_clock_o = 0xf017,
    fa_close = 0xf00d,
    fa_cloud = 0xf0c2,
    fa_cloud_download = 0xf0ed,
    fa_cloud_upload = 0xf0ee,
    fa_cny = 0xf157,
    fa_code = 0xf121,
    fa_code_fork = 0xf126,
    fa_codepen = 0xf1cb,
    fa_coffee = 0xf0f4,
    fa_cog = 0xf013,
    fa_cogs = 0xf085,
    fa_columns = 0xf0db,
    fa_comment = 0xf075,
    fa_comment_o = 0xf0e5,
    fa_comments = 0xf086,
    fa_comments_o = 0xf0e6,
    fa_compass = 0xf14e,
    fa_compress = 0xf066,
    fa_connectdevelop = 0xf20e,
    fa_copy = 0xf0c5,
    fa_copyright = 0xf1f9,
    fa_credit_card = 0xf09d,
    fa_crop = 0xf125,
    fa_crosshairs = 0xf05b,
    fa_css3 = 0xf13c,
    fa_cube = 0xf1b2,
    fa_cubes = 0xf1b3,
    fa_cut = 0xf0c4,
    fa_cutlery = 0xf0f5,
    fa_dashboard = 0xf0e4,
    fa_dashcube = 0xf210,
    fa_database = 0xf1c0,
    fa_dedent = 0xf03b,
    fa_delicious = 0xf1a5,
    fa_desktop = 0xf108,
    fa_deviantart = 0xf1bd,
    fa_diamond = 0xf219,
    fa_digg = 0xf1a6,
    fa_dollar = 0xf155,
    fa_dot_circle_o = 0xf192,
    fa_download = 0xf019,
    fa_dribbble = 0xf17d,
    fa_dropbox = 0xf16b,
    fa_drupal = 0xf1a9,
    fa_edit = 0xf044,
    fa_eject = 0xf052,
    fa_ellipsis_h = 0xf141,
    fa_ellipsis_v = 0xf142,
    fa_empire = 0xf1d1,
    fa_envelope = 0xf0e0,
    fa_envelope_o = 0xf003,
    fa_envelope_square = 0xf199,
    fa_eraser = 0xf12d,
    fa_eur = 0xf153,
    fa_euro = 0xf153,
    fa_exchange = 0xf0ec,
    fa_exclamation = 0xf12a,
    fa_exclamation_circle = 0xf06a,
    fa_exclamation_triangle = 0xf071,
    fa_expand = 0xf065,
    fa_external_link = 0xf08e,
    fa_external_link_square = 0xf14c,
    fa_eye = 0xf06e,
    fa_eye_slash = 0xf070,
    fa_eyedropper = 0xf1fb,
    fa_facebook = 0xf09a,
    fa_facebook_f = 0xf09a,
    fa_facebook_official = 0xf230,
    fa_facebook_square = 0xf082,
    fa_fast_backward = 0xf049,
    fa_fast_forward = 0xf050,
    fa_fax = 0xf1ac,
    fa_female = 0xf182,
    fa_fighter_jet = 0xf0fb,
    fa_file = 0xf15b,
    fa_file_archive_o = 0xf1c6,
    fa_file_audio_o = 0xf1c7,
    fa_file_code_o = 0xf1c9,
    fa_file_excel_o = 0xf1c3,
    fa_file_image_o = 0xf1c5,
    fa_file_movie_o = 0xf1c8,
    fa_file_o = 0xf016,
    fa_file_pdf_o = 0xf1c1,
    fa_file_photo_o = 0xf1c5,
    fa_file_picture_o = 0xf1c5,
    fa_file_powerpoint_o = 0xf1c4,
    fa_file_sound_o = 0xf1c7,
    fa_file_text = 0xf15c,
    fa_file_text_o = 0xf0f6,
    fa_file_video_o = 0xf1c8,
    fa_file_word_o = 0xf1c2,
    fa_file_zip_o = 0xf1c6,
    fa_files_o = 0xf0c5,
    fa_film = 0xf008,
    fa_filter = 0xf0b0,
    fa_fire = 0xf06d,
    fa_fire_extinguisher = 0xf134,
    fa_flag = 0xf024,
    fa_flag_checkered = 0xf11e,
    fa_flag_o = 0xf11d,
    fa_flash = 0xf0e7,
    fa_flask = 0xf0c3,
    fa_flickr = 0xf16e,
    fa_floppy_o = 0xf0c7,
    fa_folder = 0xf07b,
    fa_folder_o = 0xf114,
    fa_folder_open = 0xf07c,
    fa_folder_open_o = 0xf115,
    fa_font = 0xf031,
    fa_forumbee = 0xf211,
    fa_forward = 0xf04e,
    fa_foursquare = 0xf180,
    fa_frown_o = 0xf119,
    fa_futbol_o = 0xf1e3,
    fa_gamepad = 0xf11b,
    fa_gavel = 0xf0e3,
    fa_gbp = 0xf154,
    fa_ge = 0xf1d1,
    fa_gear = 0xf013,
    fa_gears = 0xf085,
    fa_genderless = 0xf1db,
    fa_gift = 0xf06b,
    fa_git = 0xf1d3,
    fa_git_square = 0xf1d2,
    fa_github = 0xf09b,
    fa_github_alt = 0xf113,
    fa_github_square = 0xf092,
    fa_gittip = 0xf184,
    fa_glass = 0xf000,
    fa_globe = 0xf0ac,
    fa_google = 0xf1a0,
    fa_google_plus = 0xf0d5,
    fa_google_plus_square = 0xf0d4,
    fa_google_wallet = 0xf1ee,
    fa_graduation_cap = 0xf19d,
    fa_gratipay = 0xf184,
    fa_group = 0xf0c0,
    fa_h_square = 0xf0fd,
    fa_hacker_news = 0xf1d4,
    fa_hand_o_down = 0xf0a7,
    fa_hand_o_left = 0xf0a5,
    fa_hand_o_right = 0xf0a4,
    fa_hand_o_up = 0xf0a6,
    fa_hdd_o = 0xf0a0,
    fa_header = 0xf1dc,
    fa_headphones = 0xf025,
    fa_heart = 0xf004,
    fa_heart_o = 0xf08a,
    fa_heartbeat = 0xf21e,
    fa_history = 0xf1da,
    fa_home = 0xf015,
    fa_hospital_o = 0xf0f8,
    fa_hotel = 0xf236,
    fa_html5 = 0xf13b,
    fa_ils = 0xf20b,
    fa_image = 0xf03e,
    fa_inbox = 0xf01c,
    fa_indent = 0xf03c,
    fa_info = 0xf129,
    fa_info_circle = 0xf05a,
    fa_inr = 0xf156,
    fa_instagram = 0xf16d,
    fa_institution = 0xf19c,
    fa_ioxhost = 0xf208,
    fa_italic = 0xf033,
    fa_joomla = 0xf1aa,
    fa_jpy = 0xf157,
    fa_jsfiddle = 0xf1cc,
    fa_key = 0xf084,
    fa_keyboard_o = 0xf11c,
    fa_krw = 0xf159,
    fa_language = 0xf1ab,
    fa_laptop = 0xf109,
    fa_lastfm = 0xf202,
    fa_lastfm_square = 0xf203,
    fa_leaf = 0xf06c,
    fa_leanpub = 0xf212,
    fa_legal = 0xf0e3,
    fa_lemon_o = 0xf094,
    fa_level_down = 0xf149,
    fa_level_up = 0xf148,
    fa_life_bouy = 0xf1cd,
    fa_life_buoy = 0xf1cd,
    fa_life_ring = 0xf1cd,
    fa_life_saver = 0xf1cd,
    fa_lightbulb_o = 0xf0eb,
    fa_line_chart = 0xf201,
    fa_link = 0xf0c1,
    fa_linkedin = 0xf0e1,
    fa_linkedin_square = 0xf08c,
    fa_linux = 0xf17c,
    fa_list = 0xf03a,
    fa_list_alt = 0xf022,
    fa_list_ol = 0xf0cb,
    fa_list_ul = 0xf0ca,
    fa_location_arrow = 0xf124,
    fa_lock = 0xf023,
    fa_long_arrow_down = 0xf175,
    fa_long_arrow_left = 0xf177,
    fa_long_arrow_right = 0xf178,
    fa_long_arrow_up = 0xf176,
    fa_magic = 0xf0d0,
    fa_magnet = 0xf076,
    fa_mail_forward = 0xf064,
    fa_mail_reply = 0xf112,
    fa_mail_reply_all = 0xf122,
    fa_male = 0xf183,
    fa_map_marker = 0xf041,
    fa_mars = 0xf222,
    fa_mars_double = 0xf227,
    fa_mars_stroke = 0xf229,
    fa_mars_stroke_h = 0xf22b,
    fa_mars_stroke_v = 0xf22a,
    fa_maxcdn = 0xf136,
    fa_meanpath = 0xf20c,
    fa_medium = 0xf23a,
    fa_medkit = 0xf0fa,
    fa_meh_o = 0xf11a,
    fa_mercury = 0xf223,
    fa_microphone = 0xf130,
    fa_microphone_slash = 0xf131,
    fa_minus = 0xf068,
    fa_minus_circle = 0xf056,
    fa_minus_square = 0xf146,
    fa_minus_square_o = 0xf147,
    fa_mobile = 0xf10b,
    fa_mobile_phone = 0xf10b,
    fa_money = 0xf0d6,
    fa_moon_o = 0xf186,
    fa_mortar_board = 0xf19d,
    fa_motorcycle = 0xf21c,
    fa_music = 0xf001,
    fa_navicon = 0xf0c9,
    fa_neuter = 0xf22c,
    fa_newspaper_o = 0xf1ea,
    fa_openid = 0xf19b,
    fa_outdent = 0xf03b,
    fa_pagelines = 0xf18c,
    fa_paint_brush = 0xf1fc,
    fa_paper_plane = 0xf1d8,
    fa_paper_plane_o = 0xf1d9,
    fa_paperclip = 0xf0c6,
    fa_paragraph = 0xf1dd,
    fa_paste = 0xf0ea,
    fa_pause = 0xf04c,
    fa_paw = 0xf1b0,
    fa_paypal = 0xf1ed,
    fa_pencil = 0xf040,
    fa_pencil_square = 0xf14b,
    fa_pencil_square_o = 0xf044,
    fa_phone = 0xf095,
    fa_phone_square = 0xf098,
    fa_photo = 0xf03e,
    fa_picture_o = 0xf03e,
    fa_pie_chart = 0xf200,
    fa_pied_piper = 0xf1a7,
    fa_pied_piper_alt = 0xf1a8,
    fa_pinterest = 0xf0d2,
    fa_pinterest_p = 0xf231,
    fa_pinterest_square = 0xf0d3,
    fa_plane = 0xf072,
    fa_play = 0xf04b,
    fa_play_circle = 0xf144,
    fa_play_circle_o = 0xf01d,
    fa_plug = 0xf1e6,
    fa_plus = 0xf067,
    fa_plus_circle = 0xf055,
    fa_plus_square = 0xf0fe,
    fa_plus_square_o = 0xf196,
    fa_power_off = 0xf011,
    fa_print = 0xf02f,
    fa_puzzle_piece = 0xf12e,
    fa_qq = 0xf1d6,
    fa_qrcode = 0xf029,
    fa_question = 0xf128,
    fa_question_circle = 0xf059,
    fa_quote_left = 0xf10d,
    fa_quote_right = 0xf10e,
    fa_ra = 0xf1d0,
    fa_random = 0xf074,
    fa_rebel = 0xf1d0,
    fa_recycle = 0xf1b8,
    fa_reddit = 0xf1a1,
    fa_reddit_square = 0xf1a2,
    fa_refresh = 0xf021,
    fa_remove = 0xf00d,
    fa_renren = 0xf18b,
    fa_reorder = 0xf0c9,
    fa_repeat = 0xf01e,
    fa_reply = 0xf112,
    fa_reply_all = 0xf122,
    fa_retweet = 0xf079,
    fa_rmb = 0xf157,
    fa_road = 0xf018,
    fa_rocket = 0xf135,
    fa_rotate_left = 0xf0e2,
    fa_rotate_right = 0xf01e,
    fa_rouble = 0xf158,
    fa_rss = 0xf09e,
    fa_rss_square = 0xf143,
    fa_rub = 0xf158,
    fa_ruble = 0xf158,
    fa_rupee = 0xf156,
    fa_save = 0xf0c7,
    fa_scissors = 0xf0c4,
    fa_search = 0xf002,
    fa_search_minus = 0xf010,
    fa_search_plus = 0xf00e,
    fa_sellsy = 0xf213,
    fa_send = 0xf1d8,
    fa_send_o = 0xf1d9,
    fa_server = 0xf233,
    fa_share = 0xf064,
    fa_share_alt = 0xf1e0,
    fa_share_alt_square = 0xf1e1,
    fa_share_square = 0xf14d,
    fa_share_square_o = 0xf045,
    fa_shekel = 0xf20b,
    fa_sheqel = 0xf20b,
    fa_shield = 0xf132,
    fa_ship = 0xf21a,
    fa_shirtsinbulk = 0xf214,
    fa_shopping_cart = 0xf07a,
    fa_sign_in = 0xf090,
    fa_sign_out = 0xf08b,
    fa_signal = 0xf012,
    fa_simplybuilt = 0xf215,
    fa_sitemap = 0xf0e8,
    fa_skyatlas = 0xf216,
    fa_skype = 0xf17e,
    fa_slack = 0xf198,
    fa_sliders = 0xf1de,
    fa_slideshare = 0xf1e7,
    fa_smile_o = 0xf118,
    fa_soccer_ball_o = 0xf1e3,
    fa_sort = 0xf0dc,
    fa_sort_alpha_asc = 0xf15d,
    fa_sort_alpha_desc = 0xf15e,
    fa_sort_amount_asc = 0xf160,
    fa_sort_amount_desc = 0xf161,
    fa_sort_asc = 0xf0de,
    fa_sort_desc = 0xf0dd,
    fa_sort_down = 0xf0dd,
    fa_sort_numeric_asc = 0xf162,
    fa_sort_numeric_desc = 0xf163,
    fa_sort_up = 0xf0de,
    fa_soundcloud = 0xf1be,
    fa_space_shuttle = 0xf197,
    fa_spinner = 0xf110,
    fa_spoon = 0xf1b1,
    fa_spotify = 0xf1bc,
    fa_square = 0xf0c8,
    fa_square_o = 0xf096,
    fa_stack_exchange = 0xf18d,
    fa_stack_overflow = 0xf16c,
    fa_star = 0xf005,
    fa_star_half = 0xf089,
    fa_star_half_empty = 0xf123,
    fa_star_half_full = 0xf123,
    fa_star_half_o = 0xf123,
    fa_star_o = 0xf006,
    fa_steam = 0xf1b6,
    fa_steam_square = 0xf1b7,
    fa_step_backward = 0xf048,
    fa_step_forward = 0xf051,
    fa_stethoscope = 0xf0f1,
    fa_stop = 0xf04d,
    fa_street_view = 0xf21d,
    fa_strikethrough = 0xf0cc,
    fa_stumbleupon = 0xf1a4,
    fa_stumbleupon_circle = 0xf1a3,
    fa_subscript = 0xf12c,
    fa_subway = 0xf239,
    fa_suitcase = 0xf0f2,
    fa_sun_o = 0xf185,
    fa_superscript = 0xf12b,
    fa_support = 0xf1cd,
    fa_table = 0xf0ce,
    fa_tablet = 0xf10a,
    fa_tachometer = 0xf0e4,
    fa_tag = 0xf02b,
    fa_tags = 0xf02c,
    fa_tasks = 0xf0ae,
    fa_taxi = 0xf1ba,
    fa_tencent_weibo = 0xf1d5,
    fa_terminal = 0xf120,
    fa_text_height = 0xf034,
    fa_text_width = 0xf035,
    fa_th = 0xf00a,
    fa_th_large = 0xf009,
    fa_th_list = 0xf00b,
    fa_thumb_tack = 0xf08d,
    fa_thumbs_down = 0xf165,
    fa_thumbs_o_down = 0xf088,
    fa_thumbs_o_up = 0xf087,
    fa_thumbs_up = 0xf164,
    fa_ticket = 0xf145,
    fa_times = 0xf00d,
    fa_times_circle = 0xf057,
    fa_times_circle_o = 0xf05c,
    fa_tint = 0xf043,
    fa_toggle_down = 0xf150,
    fa_toggle_left = 0xf191,
    fa_toggle_off = 0xf204,
    fa_toggle_on = 0xf205,
    fa_toggle_right = 0xf152,
    fa_toggle_up = 0xf151,
    fa_train = 0xf238,
    fa_transgender = 0xf224,
    fa_transgender_alt = 0xf225,
    fa_trash = 0xf1f8,
    fa_trash_o = 0xf014,
    fa_tree = 0xf1bb,
    fa_trello = 0xf181,
    fa_trophy = 0xf091,
    fa_truck = 0xf0d1,
    fa_try = 0xf195,
    fa_tty = 0xf1e4,
    fa_tumblr = 0xf173,
    fa_tumblr_square = 0xf174,
    fa_turkish_lira = 0xf195,
    fa_twitch = 0xf1e8,
    fa_twitter = 0xf099,
    fa_twitter_square = 0xf081,
    fa_umbrella = 0xf0e9,
    fa_underline = 0xf0cd,
    fa_undo = 0xf0e2,
    fa_university = 0xf19c,
    fa_unlink = 0xf127,
    fa_unlock = 0xf09c,
    fa_unlock_alt = 0xf13e,
    fa_unsorted = 0xf0dc,
    fa_upload = 0xf093,
    fa_usd = 0xf155,
    fa_user = 0xf007,
    fa_user_md = 0xf0f0,
    fa_user_plus = 0xf234,
    fa_user_secret = 0xf21b,
    fa_user_times = 0xf235,
    fa_users = 0xf0c0,
    fa_venus = 0xf221,
    fa_venus_double = 0xf226,
    fa_venus_mars = 0xf228,
    fa_viacoin = 0xf237,
    fa_video_camera = 0xf03d,
    fa_vimeo_square = 0xf194,
    fa_vine = 0xf1ca,
    fa_vk = 0xf189,
    fa_volume_down = 0xf027,
    fa_volume_off = 0xf026,
    fa_volume_up = 0xf028,
    fa_warning = 0xf071,
    fa_wechat = 0xf1d7,
    fa_weibo = 0xf18a,
    fa_weixin = 0xf1d7,
    fa_whatsapp = 0xf232,
    fa_wheelchair = 0xf193,
    fa_wifi = 0xf1eb,
    fa_windows = 0xf17a,
    fa_won = 0xf159,
    fa_wordpress = 0xf19a,
    fa_wrench = 0xf0ad,
    fa_xing = 0xf168,
    fa_xing_square = 0xf169,
    fa_yahoo = 0xf19e,
    fa_yelp = 0xf1e9,
    fa_yen = 0xf157,
    fa_youtube = 0xf167,
    fa_youtube_play = 0xf16a
};

}; // end class


class RepoIconEngine : public QIconEngine
{
public:

    RepoIconEngine(
            repo::gui::RepoFontAwesome* awesome,
            const QChar& iconOn,
            const QChar& iconOff,
            const QColor& colorActive = RepoFontAwesome::COLOR_NORMAL)
        : awesome(awesome)
        , iconOn(iconOn)
        , iconOff(iconOff)
        , colorActive(colorActive) {}


    virtual ~RepoIconEngine() {}

    RepoIconEngine* clone() const
    {
        return new RepoIconEngine(
                    awesome,
                    iconOn,
                    iconOff,
                    colorActive);
    }

    virtual void paint(QPainter* painter,
                       const QRect& rect,
                       QIcon::Mode mode,
                       QIcon::State state)
    {
        painter->save();
        QChar icon;
        QColor color;

        if (state == QIcon::On)
            icon = iconOn;
        else if (state == QIcon::Off)
            icon = iconOff;

        switch (mode)
        {
            case QIcon::Normal :
                color = colorActive;
                break;
            case QIcon::Disabled :
                color = QWidget().palette().dark().color(); //QColor(70,70,70,60);
                break;
            case QIcon::Active :
                color = colorActive;
                break;
            case QIcon::Selected :
                color = QWidget().palette().shadow().color(); //QColor(10,10,10);
                break;
        }

        painter->setFont(awesome->getFont(qRound(rect.height() * 0.9f)));
        painter->setPen(color);
        painter->drawText(rect, icon,
                          QTextOption(Qt::AlignCenter|Qt::AlignVCenter));
        painter->restore();
    }

    virtual QPixmap pixmap(
            const QSize &size,
            QIcon::Mode mode,
            QIcon::State state)
    {
        QPixmap pixMap(size);
        pixMap.fill(Qt::transparent);
        {
            QPainter painter(&pixMap);
            paint(&painter, QRect(QPoint(0,0), size), mode, state);
        }
        return pixMap;
    }

private:

    repo::gui::RepoFontAwesome* awesome;
    QChar iconOn;
    QChar iconOff;
    QColor colorActive;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_FONT_AWESOME_H
