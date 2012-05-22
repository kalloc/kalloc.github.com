
$(function() {
        var formcheck;
        var file;
        $.tablesorter.addParser(
            {
                id: 'position',
                is: function(s) {return s;},
                format: function(s) {
                        return parseInt(s.split(" ")[0]);
                }, 
                type: 'numeric'
            })
        var template_tab_interest = $("#tab_interest").html();
        var template_tab_groups = $("#tab_groups").html();
        var template_tab_dates = $("#tab_dates").html();
        var analyze_it = function(lj) {
                var interest = lj.interest;
                var groups = lj.groups;
                var dates = lj.dates;
                var html = "";
                //заполняем интересы
                for (var key in interest) {
                    html+='<tr>';
                    html+='<td>'+interest[key][0]+'</td>';
                    html+='<td>'+interest[key][2]+'</td>';
                    html+='<td>'+interest[key][1]+'</td>';
                    html+='</tr>';
                }
                $("#result #interest").empty();
                $("#result #interest").html(html);
                $("#result #interest").parent().tablesorter({ sortList:[[1,1]] });

                //заполняем дни рождений
                var cur_year = new Date().getFullYear();
                html = "";
                for (var key in dates) {
                    var year = parseInt(dates[key][0]);
                    html+='<tr>';
                    html+='<td>'+dates[key][0]+'</td>';
                    html+='<td>'+dates[key][2]+'%</td>';
                    html+='<td>'+dates[key][1]+'</td>';
                    html+='</tr>';
                }
                html = html.replace('&inf;','&#8734;');
                $("#result #dates").empty();
                $("#result #dates").html(html);
                $("#result #dates").parent().tablesorter({ sortList:[[1,1]] });
                //заполняем группы

                html = "";
                for (var key in groups) {
                    html+='<tr>';
                    html+='<td>'+groups[key][0]+'</td>';
                    html+='<td>'+groups[key][2]+'</td>';
                    html+='<td>'+groups[key][1]+'</td>';
                    html+='</tr>';
                }
                $("#result #groups").empty();
                $("#result #groups").html(html);
                $("#result #groups").parent().tablesorter({ sortList:[[1,1]] });
                //показываем чудо

                $("#shot #notice > span").html("Работа закончена.");
                $("#result").show('slow');

        }

        $('#shot').submit(function() {
                var tryLoad = 0;
                $("#result").hide('slow');
                $("#tab_interest").html(template_tab_interest);
                $("#tab_groups").html(template_tab_groups);
                $("#tab_dates").html(template_tab_dates);
                $("#shot #error").hide();
                var success = function(cur) {
                    tryLoad+=1;
                    if(tryLoad == 10) {
                        cur.error="ошибка загрузки. превышено время ожидания.";
                    }
                    $("#shot > div > div").hide()
                    if(cur.groups) {
                        $("#shot #notice > span").html("Ответ получен - обработка.");
                        analyze_it(cur);
                    } else if (cur.processed == -1)  {
                        $("#shot #error > span").html('пользователь не найден');
                        $("#shot #notice > span").html("");
                        $("#shot #error").show();

                    } else {

                        if(cur.processed ) {
                            $("#shot #notice > span").html("Процент выполнения - "+parseFloat(cur.processed).toString()+' %');
                        } else {
                            $("#shot #notice > span").html("Ваш запрос поставлен в очередь.");
                        }
                        $("#shot #notice").show();
                        setTimeout(function() {
                            var user = $('#shot').find("input").val();
                            document.location.hash=user;
                            if(!cur.processed) {
                                $("#shot #notice > span").html("отправка запроса");
                            }
                            $("#shot #notice").show();
                            $.ajax({
                                    type: "POST",   url:'/json/lj', dataType: 'json',
                                    data: {'ljuser':user},
                                    async:true,
                                    success:success,
                                    error: function() {
                                    }
                                });
                        },1000);
                    }
                }
                success([]);
            return false;
        });

        if(location.hash != '') {
            var user = location.hash.replace('#','');
            $('#shot').find("input").val(user);
            $('#shot').submit();
        }

        $("#tabs").tabs();
});


