quest discord_level begin
	state start begin
		when levelup with pc.get_level() == 105 begin
			-- token_index, channel_index, message
			discord_message(0, 1, string.format("**%s** has reached `%d` level!", pc.get_name(), pc.get_level()))
		end
	end
end