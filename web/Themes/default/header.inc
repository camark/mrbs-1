<?php

// $Id: header.inc 2784 2013-11-21 10:48:22Z cimorrison $


// Print the page header
function print_theme_header($day, $month, $year, $area, $room)
{
    global $mrbs_company, $mrbs_company_logo, $mrbs_company_url, $mrbs_company_more_info,
           $search_str, $locale_warning;
    global $tbl_entry, $tbl_room, $tbl_area;
    global $PHP_SELF, $HTTP_HOST, $QUERY_STRING;
    global $view_week_number, $weekstarts, $times_along_top, $periods, $enable_periods;
    global $auth, $max_level;
    global $default_language_tokens, $disable_automatic_language_changing, $override_locale;
    global $select_options;
    global $ajax_refresh_rate;
    global $main_table_cell_border_width, $main_cell_height;
    global $timetohighlight;

    $page = basename($PHP_SELF, ".php");
    $user = getUserName();
    $is_admin = (authGetUserLevel($user) >= $max_level);

    // Need to set the timezone before we can use date()
    get_area_settings($area);

    // If we dont know the right date then make it up
    if (!$day) {
        $day = date("d");
    }
    if (!$month) {
        $month = date("m");
    }
    if (!$year) {
        $year = date("Y");
    }
    if (!isset($search_str)) {
        $search_str = "";
    }
    // searching box function
    function searchBox()
    {
        global $search_str, $day, $month, $year, $area, $room;
        $special_search_str = htmlspecialchars($search_str);
        $search_text = get_vocab("search");
        $hiddenInput = '';
        if (!empty($area)) {
            $hiddenInput = "<input type=\"hidden\" name=\"area\" value=\"{$area}\">\n";
        }
        if (!empty($room)) {
            $hiddenInput = "<input type=\"hidden\" name=\"room\" value=\"{$room}\">\n";
        }

        $output = <<< EOF
        <form id="header_search" method="get" action="search.php"  class="navbar-form navbar-left col-md-4" role="search">
            <div class="input-group input-group-sm">
            <span class="input-group-addon"><a href="search.php?advanced=1">{$search_text}:</a></span>

                <input class="form-control" placeholder="Search" type="search" name="search_str" value="{$special_search_str}" required>
                <span class="input-group-btn">
                    <button type="submit" class="btn btn-default">Submit</button>
                </span>
            </div>
                <input type="hidden" name="day"        value="{$day} ?>">
                <input type="hidden" name="month"      value="{$month}">
                <input type="hidden" name="year"       value="{$year}">

                {$hiddenInput}
        </form>
EOF;
        echo $output;
    }

    function pickDate()
    {
        global $day, $month, $year, $area, $room, $user;
        global $tbl_entry, $tbl_room, $tbl_area, $is_admin;

        echo '<form action="day.php" method="get" id="Form1"  class="navbar-form navbar-left form-inline" role="form">';
        // Give the form id as the optional fifth parameter because we want
        // the form to be automatically submitted when the datepicker is closed
        genDateSelector("", $day, $month, $year, "Form1");
        if (!empty($area)) {
            echo "<input type=\"hidden\" name=\"area\" value=\"$area\">\n";
        }
        if (!empty($room)) {
            echo "<input type=\"hidden\" name=\"room\" value=\"$room\">\n";
        }

        // Although the datepicker will automatically go to the new date when
        // the date is changed, we still need a submit button because there
        // are occasions when you want to go to the date without changing it -
        // for example when you've been on a Search or Report page
        echo '<button type="submit" class="btn btn-primary btn-sm" value="' . get_vocab("goto") . '">Submit</button>';
        echo '</form>';

        // Provide a link to the list of bookings awaiting approval
        // (if there are any enabled areas where we require bookings to be approved)

        $approval_somewhere = some_area('approval_enabled', TRUE);
        if ($approval_somewhere && (authGetUserLevel($user) >= 1)) {
            $sql_approval_enabled = some_area_predicate('approval_enabled');
            // Find out how many bookings are awaiting approval
            // (but only for areas where approval is required)

            $sql = "SELECT COUNT(*) FROM $tbl_entry E, $tbl_room R, $tbl_area A
                       WHERE (status&" . STATUS_AWAITING_APPROVAL . " != 0)
                          AND E.room_id = R.id
                          AND R.area_id = A.id
                          AND R.disabled = 0
                          AND A.disabled = 0
                          AND $sql_approval_enabled";
            if (!$is_admin) {
                // Ordinary users can only see their own
                $sql .= " AND create_by='" . sql_escape($user) . "'";
            }
            $n_outstanding = sql_query1($sql);

            if ($n_outstanding < 0) {
                trigger_error(sql_error(), E_USER_WARNING);
                fatal_error(FALSE, get_vocab("fatal_db_error"));
            }
            $n_outstandingOut = (($n_outstanding > 0) ? " class=\"outstanding\"" : '');
            $roomOut = ((!empty($room)) ? "&amp;room=$room" : "");
            $n_outstandingText = $n_outstanding . get_vocab("outstanding");

            echo '<div id="n_outstanding" ' . $n_outstandingOut . '>';
            echo '<a href="pending.php?day=' . $day . '&month=' . $month . '&year=' . $year . '&area=' . $area . $roomOut . '>'
                . $n_outstandingText .
                '</a>';
            echo '</div>';
        }
    }

    http_headers();
    echo DOCTYPE;
    ?>

    <!DOCTYPE html>
    <html lang="en">
    <head>

        <meta charset="<?php echo get_charset(); ?>">
        <title><?php echo get_vocab("mrbs") ?></title>
        <meta charset="UTF-8">
        <meta name=description content="">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">

        <?php
        require_once "style.inc";
        require_once "js.inc";
        ?>
    </head>

    <?php
    // Put the filename in as a class to aid styling.
    // (Use a class rather than id to avoid specificity problems)
    echo "<body class=\"non_js " . htmlspecialchars($page) . "\">\n";

    // Add a class of "js" so that we know if we're using JavaScript or not
    // and remove the non_js class (it's sometimes useful to know that we're
    // not running JavaScript)
    ?>
    <script type="text/javascript">
        //<![CDATA[
        $('body').addClass('js').removeClass('non_js');
        //]]>
    </script>

<div class="screenonly">

    <?php // show a warning if this is using a low version of php
    if (substr(phpversion(), 0, 1) == 3) {
        ?>
        <div class="alert alert-warning">
            <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
            <strong>Notice!</strong> <?php echo get_vocab("not_php3");?>
        </div>
    <?php
    }
    if (!empty($locale_warning)) {
        ?>
        <div class="alert alert-warning">
            <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
            <strong>Warning !</strong> <?php echo $locale_warning;?>
        </div>
    <?php
    }
    ?>
    <div class="wrapper">
        <nav class="navbar navbar-default">
            <div class="container-fluid">
                <!-- Brand and toggle get grouped for better mobile display -->
                <div class="navbar-header">
                    <button type="button" class="navbar-toggle collapsed" data-toggle="collapse"
                            data-target="#bs-example-navbar-collapse-1">
                        <span class="sr-only">Toggle navigation</span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                    </button>
                    <a class="navbar-brand col-md-4" href="/">
                        <?php
                        if (empty($mrbs_company_logo)) {
                            echo "<span>$mrbs_company</span>\n";
                        } else {
                            // Suppress error messages in case the logo is a URL and allow_url_fopen
                            // is not enabled in php.ini
                            $logo_size = @getimagesize($mrbs_company_logo);
                            echo "<img src=\"$mrbs_company_logo\" " . $logo_size[3] . " alt=\"$mrbs_company\">\n";
                        }
                        if (!empty($mrbs_company_more_info)) {
                            echo "$mrbs_company_more_info";
                        }
                        ?>
                    </a>
                    <a href="index.php"><?php echo get_vocab("mrbs") ?></a>
                </div>

                <!-- Collect the nav links, forms, and other content for toggling -->
                <div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
                    <ul class="nav navbar-nav">
                        <?php
                        $query_str = "day=$day&amp;month=$month&amp;year=$year";
                        if (!empty($area)) {
                            $query_str .= "&amp;area=$area";
                        }
                        if (!empty($room)) {
                            $query_str .= "&amp;room=$room";
                        }
                        echo "<li><a href=\"admin.php?$query_str\">" . get_vocab("rooms") . "</a></li>";
                        echo "<li><a href=\"report.php?$query_str\">" . get_vocab("report") . "</a></li>";
                        echo "<li>" . userlistURL() . "</li>";

                        echo "<li><a href=\"help.php?$query_str\">" . get_vocab("help") . "</a></li>";

                        if (isset($user_list_link)) {
                            echo "<li><a href=\"$user_list_link\">" . get_vocab('user_list') . "</a></li>";
                        }
                        ?>
                    </ul>
                    <?php
                    pickDate();
                    ?>
                    <?php
                    searchBox();
                    ?>
                    <div class="collapse navbar-collapse" id="bs-example-navbar-collapse-5">
                        <p class="navbar-text navbar-right">
                            <?php echo PrintLogonBoxOnly(); ?></p>
                    </div>
                </div>
                <!-- /.navbar-collapse -->
            </div>
            <!-- /.container-fluid -->
        </nav>


    <div id="contents">
        <div class="container-fluid">
            <div class="row">
    <?php
} // end of print_theme_header()

