// Qiksearch Menu v 1.0
// (c) 2002 Premshree Pillai
// http://www.qiksearch.com
// qiksearch@rediffmail.com

var links = new Array (
	"KVMultiDet_ChangeLog",
	"KVVarGlob_ChangeLog",
	"KVTrieur_ChangeLog",
	"KVIndra_ChangeLog",
	"KVIndra5_ChangeLog",
	"KVIndraFNL_ChangeLog",
	"KaliVeda_ChangeLog"
);

var links_text = new Array (
	"libKVMultiDet",
	"libKVVarGlob",
	"libKVTrieur",
	"libKVIndra",
	"libKVIndra5",
	"libKVIndraFNL",
	"KaliVeda4INDRA"
 );

var links_url = new Array (
	"KVMultiDet_ChangeLog.html",
	"KVVarGlob_ChangeLog.html",
	"KVTrieur_ChangeLog.html",
	"KVIndra_ChangeLog.html",
	"KVIndra5_ChangeLog.html",
	"KVIndraFNL_ChangeLog.html",
	"KaliVeda_ChangeLog.html"
 );

var loc=String(this.location);
loc=loc.split("/");
loc=loc[loc.length-1].split(".");
loc=loc[loc.length-2];

function qiksearch_menu_gen()
{
 for(var i=0; i<links.length; i++)
 {
  if(((this.location=="http://www.qiksearch.com") || (this.location=="http://www.qiksearch.com/") || (this.location=="http://qiksearch.com") || (this.location=="http://qiksearch.com/")) && (i==0))
{
   document.write('<table class="explorer_active" onmouseover="this.className=&#39;explorer_active&#39;;return true" onmouseout="this.className=&#39;explorer_active&#39;;return true" onmousedown="this.className=&#39;explorer_active&#39;;return true" onclick="location.href=&#39;' + links_url[i] + '&#39;"><tr><td><a href="' + links_url[i] + '" class="menu">' + links_text[i] + ' <b>&raquo;</b></a></td></tr></table>');
}
else
{
  if(loc==links[i])
  {
   document.write('<table class="explorer_active" onmouseover="this.className=&#39;explorer_active&#39;;return true" onmouseout="this.className=&#39;explorer_active&#39;;return true" onmousedown="this.className=&#39;explorer_active&#39;;return true" onclick="location.href=&#39;' + links_url[i] + '&#39;"><tr><td><a href="' + links_url[i] + '" class="menu">' + links_text[i] + ' <b>&raquo;</b></a></td></tr></table>');
  }
  else
  {
   document.write('<table class="explorer" onmouseover="this.className=&#39;explorer_over&#39;;return true" onmouseout="this.className=&#39;explorer&#39;;return true" onmousedown="this.className=&#39;explorer_down&#39;;return true" onclick="location.href=&#39;' + links_url[i] + '&#39;"><tr><td><a href="' + links_url[i] + '" class="menu">' + links_text[i] + '</a></td></tr></table>');
  }
 }
  document.write('<table cellspacing="0" cellpadding="0" bgcolor="#FFFFFF"><tr><td></td></tr></table>');
 }
} 

qiksearch_menu_gen();

// Temp Script
/*
var check_ns4=(document.layers)?true:false;

if(!check_ns4)
{
 document.writeln('<scr' + 'ipt language="JavaScript" src="scripts/JSdragTemp.js">' + '<' + '/' + 'scr' + 'ipt>');
}
*/
