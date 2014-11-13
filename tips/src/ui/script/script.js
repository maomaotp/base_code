$(function () {
	  //列表中点击+、-号，展开和收起部分信息	  $(".openCloseImg").bind("click", function () {	  if (this.src.indexOf("img/style/close.png")!=-1) {	       this.src = "img/style/open.png";	   } else {	      this.src = "img/style/close.png";	  }	   $(this).parents("td").parents("tr").nextAll("tr").first().toggle();	   });


	    //当前请求的url
	    var currentUrl = window.location;
	    tokenCurrntMenu(currentUrl);


	//列表的交替行颜色处理
	/*奇数行*/
	$(".list tr:nth-child(odd)").css("background-color","#f8f8f8");
	/*偶数行*/
	$(".list tr:nth-child(even)").css("background-color","#f4f4f4");

	/*有折叠行的tale*/
	$(".openCloseList tr:nth-child(4n+1)").css("background-color","#f4f4f4");
	$(".openCloseList tr:nth-child(4n-1)").css("background-color","#f8f8f8");

 });
//设置当前导航菜单的当前项，根据url中cmd参数值判断当前页面
function tokenCurrntMenu(currentUrl){

    var id= "";
    //url 中cmd参数值
    var cmdStr=getQueryString("cmd");

    //用户管理
    if (cmdStr=="get_users"||cmdStr=="user_delete"||cmdStr=="user_modify_page"||cmdStr=="tips_create_user") {
        id = "1";
    }
    //配置管理
    else if (cmdStr=="get_config"|| cmdStr=="configadmin"){
        id = "2";
    }
    //数据管理
    else if (cmdStr=="get_data" || cmdStr=="data_display" || cmdStr=="data_delete") {
        id = "3";
    }
    //审计查询
    else if ( cmdStr=="get_audit"||cmdStr=="tips_opt_copy"||cmdStr=="tips_opt_display") {
        id = "4";
    }
  //检索信息--攻击窃密
    else if ( cmdStr=="get_Alarm_info"||cmdStr=="tips_alarm_display" ||cmdStr=="tips_alarm_opt") {
        id = "5";
    }
  //检索信息--涉密标识事件
    else if ( cmdStr=="get_event_info"||cmdStr=="tips_event_display"||cmdStr=="tips_event_opt") {
        id = "6";
    }
   //检索信息--上网行为审计
   else if ( cmdStr=="get_netlog_info"||cmdStr=="tips_netlog_display"||cmdStr=="tips_netlog_opt") {
        id = "7";
    }
    //产品维护--设备状态
    else if ( cmdStr=="get_product"||cmdStr=="upgrade"||cmdStr=="debuging") {
        id = "8";
    }
  //产品维护--系统设置
    else if ( cmdStr=="get_sysSetting"||cmdStr=="upgrade_1"|| cmdStr=="debuging_1"|| cmdStr=="system_config") {
        id = "9";
    }
   //统计信息--攻击窃密
    else if ( cmdStr=="get_information_attack") {
        id = "10";
    }
  //统计信息--操作审计
  else if ( cmdStr=="get_information_operation")   {
        id = "11";
    }
  //统计信息--涉密标识
  else if ( cmdStr=="get_information_secret") {
        id = "12";
    }
  //统计信息--行为审计
   else if (cmdStr=="get_information_action") {
        id = "13";
    }
    addCss(id);

}

function addCss(id) {
    if (id == "") {
        return;
    }
    //让本节点所在的li增加当前样式，兄弟节点去掉当前样式
  	var current=  $(".menu ul.grade_1 li ul #" + id);

	current.addClass("g2_current");
	current.siblings().removeClass("g2_current");


    //所在的一级菜单 增加当前样式,一级菜单的兄弟节点去掉当前样式
   current.parents("ul").parents("li").addClass("current");
   current.parents("ul").parents("li").siblings().removeClass("current");    
}

//获取url参数
function getQueryString(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)", "i");
    var r = window.location.search.substr(1).match(reg);
    if (r != null) return unescape(r[2]); return null;
}
