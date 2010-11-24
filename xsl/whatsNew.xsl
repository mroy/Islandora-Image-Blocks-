<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:fn="http://www.w3.org/2005/xpath-functions" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:php="http://php.net/xsl" exclude-result-prefixes="php">
 <!-- Red and White XSLT -->
 <xsl:variable name="BASEURL">
  <xsl:value-of select="$baseUrl"/>
 </xsl:variable>
 <xsl:variable name="PATH">
  <xsl:value-of select="$path"/>
 </xsl:variable>
 <xsl:template match="/response/lst"/>
 <xsl:template match="/response/result/doc">
   <xsl:element name="li">
     <xsl:element name="img">
       <xsl:attribute name="class">whatsNewThumb</xsl:attribute>
       <xsl:attribute name="src">
	 <xsl:value-of select="concat($BASEURL,'/fedora/repository/',translate(arr[@name='id']/str,'/',':'),'/TN')"/>
       </xsl:attribute>
     </xsl:element>
     <div>
       <xsl:element name="a">
	 <xsl:attribute name="href">
	   <xsl:value-of select="concat($BASEURL,'/fedora/repository/',translate(arr[@name='id']/str,'/',':'))"/>
	 </xsl:attribute>     
       <xsl:value-of select="arr[@name='title_t']/str"/>
       </xsl:element>
       <xsl:value-of select="php:function('date',' M j, Y', php:function('strtotime',string(arr[@name='created_d'])))"/>
     </div>
    
    </xsl:element>
 </xsl:template>
 
</xsl:stylesheet>
  