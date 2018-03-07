// Qiksearch Menu v 1.0
// (c) 2002 Premshree Pillai
// http://www.qiksearch.com
// qiksearch@rediffmail.com

var links = new Array (
  "index",
  "download",
  "build",
//  "kvrootrc",
//  "DataAnalysis",
//  "Examples",
//  "KaliVeda-FAQ",
  "ClassRef",
  "usersguide",
  "appli",
  "github",
  "KaliVedaBugs"
 );

var links_text = new Array (
  "Home",
  "Download",
  "Build & Install",
//  "Configuration",
//  "Data Analysis",
//  "Examples",
//  "Tutorials",
  "Class Reference",
  "Users Guide",
  "Applications",
  "Github",
  "Report a Bug"
 );

var links_url = new Array (
  "index.html",
  "download.html",
  "build.html",
//  "kvrootrc.html",
//  "DataAnalysis.html",
//  "Examples.html",
//  "KaliVeda-FAQ.html",
  "http://kaliveda-dev.github.io/kaliveda",
  "usersguide.html",
  "appli.html",
  "https://github.com/kaliveda-dev/kaliveda",
  "https://github.com/kaliveda-dev/kaliveda/issues"
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

//function used by 'Search KaliVeda website' form
function send_data(formname)
	{
		formname.q.value+=" site:indra.in2p3.fr inurl:kaliveda";
		return true;
	}
   
