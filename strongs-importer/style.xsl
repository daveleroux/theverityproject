<xsl:stylesheet
                  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                  version="2.0">
            <xsl:output  method="xml"
                        indent="yes"
                        omit-xml-declaration="yes"/>

			<xsl:template match="/">
				<entry>
				<heading>Strong's Definition</heading>
					<xsl:apply-templates/>
				</entry>
			</xsl:template>

			<xsl:template match="/entry">
				<xsl:apply-templates/>
			</xsl:template>

            <xsl:template  match="*">
                  <xsl:copy>
                        <xsl:apply-templates/>
                  </xsl:copy>
            </xsl:template>

            <xsl:template  match="*[name()='strongsref']">            				
				<xsl:element name="{@language}">
					<xsl:element  name="link" >
						<xsl:attribute name="id"><xsl:value-of select="@strongs_number"/></xsl:attribute>  
						<xsl:value-of select="@lemma"/>                     
					</xsl:element>
   		        </xsl:element>
            </xsl:template>

            <xsl:template  match="*[name()='see']">
				<compare>
				Compare:
					<xsl:element name="{@language}">
						<xsl:element  name="link" >
							<xsl:attribute name="id"><xsl:value-of select="@strongs_number"/></xsl:attribute>  
							<xsl:value-of select="@lemma"/>                     
						</xsl:element>
					</xsl:element>
				</compare>
            </xsl:template>


            <xsl:template  match="*[name()='latin']">
                  <xsl:element  name="i" >
                        <xsl:apply-templates/>
                  </xsl:element>
            </xsl:template>

            <xsl:template  match="*[name()='strongs_definition']">
                  <xsl:element  name="b" >
                        <xsl:apply-templates/>
                  </xsl:element>
            </xsl:template>

            <xsl:template  match="*[name()='kjv_definition']">
				<heading>KJV Definition</heading>
                        <xsl:apply-templates/> 		  
            </xsl:template>

      </xsl:stylesheet> 
